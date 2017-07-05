#include <deque>
#include <unordered_map>
#include <cmath>
#include "Simulation.h"

const double Simulation::G = 0.00000000006674; //6.674*10^-11
const double Simulation::PI = 3.141592653589793238463;

namespace
{
	static const double D_RAND_MAX = static_cast<double>(RAND_MAX);

	// Returns a random value in the range (-base*size/2, base*size/2)
	double variance(double base, double size)
	{
		double rand = static_cast<double>(std::rand()) / D_RAND_MAX;
		return (base * size) * (rand - 0.5);
	}

	Vector3d random_step(double speed, double distribution, double step_size)
	{
		// Get random direction
		double x, y, z;
		// Loop because direction must not be all zeroes
		do
		{
			x = (static_cast<double>(std::rand()) / D_RAND_MAX) - 0.5;
			y = (static_cast<double>(std::rand()) / D_RAND_MAX) - 0.5;
			z = (static_cast<double>(std::rand()) / D_RAND_MAX) - 0.5;
		}
		while(x == 0.0 && y == 0.0 && z == 0.0);
		Vector3d direction(x, y, z);
		direction.normalize();

		// Get random speed
		double rand_speed = variance(speed, distribution);

		// Return step of the correct size
		return direction * (rand_speed * step_size);
	}
}

Simulation::Simulation(const SimulationInitialConditions& cond)
: STEPSIZE(cond.step_size), m_bodies()
{
	std::srand(cond.random_seed);

	const double dist = cond.distribution;
	const double hd = dist / 2.0;
	const double average_mass = cond.system_mass / cond.number_of_bodies;
	// Due to rounding we'll likely be placing a bit too few bodies. But who's counting?
	const int quadrant_side = static_cast<int>(std::sqrt(cond.number_of_bodies / 4.0));
	for(int i = 0; i < quadrant_side; ++i)
	{
		for(int j = 0; j < quadrant_side; ++j)
		{
			const double x_base = hd + i*cond.distribution;
			const double y_base = hd + j*cond.distribution;

			for(int k = 0; k < 4; ++k)
			{
				double x_pos = x_base + variance(cond.distribution, cond.distribution_variance);
				double y_pos = y_base + variance(cond.distribution, cond.distribution_variance);
				Vector3d position(0.0, 0.0, 0.0);

				switch(k)
				{
				case 0:
					// First quadrant +x +y
					position = Vector3d(x_pos, y_pos, 0.0);
					break;
				case 1:
					// Second quadrant +x -y
					position = Vector3d(x_pos, -y_pos, 0.0);
					break;
				case 2:
					// Third quadrant -x -y
					position = Vector3d(-x_pos, -y_pos, 0.0);
					break;
				case 3:
					// Forth quadrant -x +y
					position = Vector3d(-x_pos, y_pos, 0.0);
					break;
				}
				double mass = average_mass + variance(average_mass, cond.mass_variance);
				Vector3d previous_pos = position + random_step(cond.speed, cond.speed_variance, STEPSIZE);
				m_bodies.emplace_back(position, previous_pos, mass);
			}
		}
	}
}

void Simulation::simulate(int steps)
{
	for(int i = 0; i < steps; ++i)
	{
		calculate_gravity();
		integrate();
		handle_collisions();
		m_bodies.erase(std::remove_if(m_bodies.begin(), m_bodies.end(), [](const Body& b){return b.remove;}), m_bodies.end());
	}
}

void Simulation::draw(Graphics& drawer)
{
	for(Body& i : m_bodies)
	{
		drawer.draw_sphere(i.position, i.radius);
	}
}

Vector3d Simulation::get_system_velocity() const
{
	Vector3d velocity(0.0, 0.0, 0.0);
	double system_mass = 0.0;
	for(const Body& i : m_bodies)
	{
		Vector3d i_vel = (i.position - i.previous_position) * (1.0 / STEPSIZE);
		velocity += i_vel * i.mass;
		system_mass += i.mass;
	}
	velocity *= 1.0 / system_mass;
	return velocity;
}

int Simulation::get_body_count() const
{
	return m_bodies.size();
}

void Simulation::calculate_gravity()
{
	const unsigned int body_count = m_bodies.size();
	for(unsigned int i = 0; i < body_count; ++i)
	{
		Body& I = m_bodies[i];
		for(unsigned int j = (i + 1); j < body_count; ++j)
		{
			Body& J = m_bodies[j];
			Vector3d direction = J.position - I.position;
			double distance_squared = direction.length_squared();
			direction.normalize();
			Vector3d force = direction * (G * I.mass * J.mass / distance_squared);
			I.incoming_force += force;
			J.incoming_force -= force;
		}
	}
}

void Simulation::integrate()
{
	// Using Störmer-Verlet, because velocity is lame
	for(Body& i : m_bodies)
	{
		Vector3d acceleration = i.incoming_force * i.inverse_mass;
		i.incoming_force = Vector3d(0.0, 0.0, 0.0);
		Vector3d saved_pos(i.position);
		i.position += i.position - i.previous_position + acceleration * (STEPSIZE * STEPSIZE);
		i.previous_position = saved_pos;
	}
}

void Simulation::handle_collisions()
{
	typedef std::vector<Body*> MergeList;
	// Important! Deque does not invalidate member pointers on insert
	std::deque<MergeList> merge_lists;
	std::unordered_map<unsigned int, MergeList*> merge_look_up;
	const unsigned int body_count = m_bodies.size();
	// Identify colliding objects and put them in together in "merge lists"
	for(unsigned int i = 0; i < body_count ; ++i)
	{
		Body& I = m_bodies[i];
		for(unsigned int j = (i + 1); j < body_count; ++j)
		{
			Body& J = m_bodies[j];
			double distance = (J.position - I.position).length();
			if(distance <= J.radius + I.radius)
			{
				// Check if they already are to be merged
				bool i_merging = merge_look_up.count(i);
				bool j_merging = merge_look_up.count(j);
				if(!i_merging && !j_merging)
				{
					// Create new merge list
					merge_lists.emplace_back(std::initializer_list<Body*>{&I, &J});
					MergeList* mlp = &merge_lists.back();
					merge_look_up[i] = mlp;
					merge_look_up[j] = mlp;
				}
				else if(i_merging && !j_merging)
				{
					// Add j to i's merge list
					merge_look_up[i]->push_back(&J);
				}
				else if(!i_merging && j_merging)
				{
					// Add i to j's merge list
					merge_look_up[j]->push_back(&I);
				}
				else //if(i_merging && j_merging) We don't need this check
				{
					MergeList* mlp1 = merge_look_up[i];
					MergeList* mlp2 = merge_look_up[j];
					if(mlp1 != mlp2)
					{
						// They are in different merge lists
						// Merge the two merge lists
						mlp1->reserve(mlp1->size() + mlp2->size());
						mlp1->insert(mlp1->end(), mlp2->begin(), mlp2->end());
						mlp2->clear();
						merge_look_up[j] = mlp1;
					}
				}
			}
		}
	}

	// Merge all colliding objects
	for(MergeList& ml : merge_lists)
	{
		Body new_body({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0.0);
		for(Body* body : ml)
		{
			new_body.mass += body->mass;
		}
		new_body.inverse_mass = 1.0 / new_body.mass;
		// We could do this with just one loop and divide by total mass in the end
		// but that might result in precision problems as it would give very large values before division
		for(Body* body : ml)
		{
			body->remove = true; // Mark old bodies for deletion
			new_body.position += (new_body.inverse_mass * body->mass) * body->position;
			new_body.previous_position += (new_body.inverse_mass * body->mass) * body->previous_position;
		}
		new_body.radius = radius_from_mass(new_body.mass);
		m_bodies.push_back(new_body);
	}
}

Simulation::Body::Body(Vector3d position, Vector3d previous_position, double mass)
: position(position),
  previous_position(previous_position),
  incoming_force(0.0, 0.0, 0.0),
  mass(mass),
  inverse_mass(mass > 0.0 ? 1.0/mass : 0.0), // Don't divide by zero
  radius(radius_from_mass(mass)),
  remove(false)
{
}

double Simulation::radius_from_mass(double mass)
{
	// Assuming homogeneous objects with a density of p kg/m^3 gives
	// volume * p = mass <-> volume = mass/p
	// Sphere volume formula gives
	// volume = (4Pi*r^2)/3 <-> sqrt((3*volume)/(4Pi)) = r
	// Substitution gives
	// sqrt((3*mass/p)/(4Pi)) = r <-> sqrt((3*mass)/(4Pi*p)) = r
	// According to Kokubo & Ida 2012 planetesimals have a density of 2 g/cm^3
	// 2 g/cm^3 = 2000 kg/m^3
	double p = 2000;
	return std::sqrt((3.0 * mass) / (4.0 * PI * p));
}
