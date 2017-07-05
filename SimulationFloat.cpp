#include <deque>
#include <unordered_map>
#include <cmath>
#include "Vector3d.h"
#include "SimulationFloat.h"

const float SimulationFloat::G = 0.00000000006674f; //6.674*10^-11
const float SimulationFloat::PI = 3.141592653589793238463f;

namespace
{
	static const float D_RAND_MAX = static_cast<float>(RAND_MAX);

	// Returns a random value in the range (-base*size/2, base*size/2)
	float variance(float base, float size)
	{
		float rand = static_cast<float>(std::rand()) / D_RAND_MAX;
		return (base * size) * (rand - 0.5f);
	}

	Vector3f random_step(float speed, float distribution, float step_size)
	{
		// Get random direction
		float x, y, z;
		// Loop because direction must not be all zeroes
		do
		{
			x = (static_cast<float>(std::rand()) / D_RAND_MAX) - 0.5f;
			y = (static_cast<float>(std::rand()) / D_RAND_MAX) - 0.5f;
			z = (static_cast<float>(std::rand()) / D_RAND_MAX) - 0.5f;
		}
		while(x == 0.0f && y == 0.0f && z == 0.0f);
		Vector3f direction(x, y, z);
		direction.normalize();

		// Get random speed
		float rand_speed = variance(speed, distribution);

		// Return step of the correct size
		return direction * (rand_speed * step_size);
	}
}

SimulationFloat::SimulationFloat(const SimulationFloatInitialConditions& cond)
: STEPSIZE(cond.step_size), m_bodies()
{
	std::srand(cond.random_seed);

	const float dist = cond.distribution;
	const float hd = dist / 2.0f;
	const float average_mass = cond.system_mass / cond.number_of_bodies;
	// Due to rounding we'll likely be placing a bit too few bodies. But who's counting?
	const int quadrant_side = static_cast<int>(std::sqrt(cond.number_of_bodies / 4.0));
	for(int i = 0; i < quadrant_side; ++i)
	{
		for(int j = 0; j < quadrant_side; ++j)
		{
			const float x_base = hd + i*cond.distribution;
			const float y_base = hd + j*cond.distribution;

			for(int k = 0; k < 4; ++k)
			{
				float x_pos = x_base + variance(cond.distribution, cond.distribution_variance);
				float y_pos = y_base + variance(cond.distribution, cond.distribution_variance);
				Vector3f position(0.0, 0.0, 0.0);

				switch(k)
				{
				case 0:
					// First quadrant +x +y
					position = Vector3f(x_pos, y_pos, 0.0f);
					break;
				case 1:
					// Second quadrant +x -y
					position = Vector3f(x_pos, -y_pos, 0.0f);
					break;
				case 2:
					// Third quadrant -x -y
					position = Vector3f(-x_pos, -y_pos, 0.0f);
					break;
				case 3:
					// Forth quadrant -x +y
					position = Vector3f(-x_pos, y_pos, 0.0f);
					break;
				}
				float mass = average_mass + variance(average_mass, cond.mass_variance);
				Vector3f previous_pos = position + random_step(cond.speed, cond.speed_variance, STEPSIZE);
				m_bodies.emplace_back(position, previous_pos, mass);
			}
		}
	}
}

void SimulationFloat::simulate(int steps)
{
	for(int i = 0; i < steps; ++i)
	{
		calculate_gravity();
		integrate();
		handle_collisions();
		m_bodies.erase(std::remove_if(m_bodies.begin(), m_bodies.end(), [](const Body& b){return b.remove;}), m_bodies.end());
	}
}

void SimulationFloat::draw(Graphics& drawer)
{
	for(Body& i : m_bodies)
	{
		// Graphics wants a double vector
		Vector3d double_pos(i.position.get_x(), i.position.get_y(), i.position.get_z());
		drawer.draw_sphere(double_pos, i.radius);
	}
}

Vector3d SimulationFloat::get_system_velocity() const
{
	// These calculations are done with doubles in order to minimize precision loss
	Vector3d velocity(0.0, 0.0, 0.0);
	double system_mass = 0.0;
	for(const Body& i : m_bodies)
	{
		Vector3d i_pos(i.position.get_x(), i.position.get_y(), i.position.get_z());
		Vector3d i_prev_pos(i.previous_position.get_x(), i.position.get_y(), i.position.get_z());
		Vector3d i_vel = (i_pos - i_prev_pos) * (1.0 / STEPSIZE);

		velocity += i_vel * i.mass;
		system_mass += i.mass;
	}
	velocity *= 1.0 / system_mass;
	return velocity;
}

int SimulationFloat::get_body_count() const
{
	return m_bodies.size();
}

void SimulationFloat::calculate_gravity()
{
	const unsigned int body_count = m_bodies.size();
	for(unsigned int i = 0; i < body_count; ++i)
	{
		Body& I = m_bodies[i];
		for(unsigned int j = (i + 1); j < body_count; ++j)
		{
			Body& J = m_bodies[j];
			Vector3f direction = J.position - I.position;
			float distance_squared = direction.length_squared();
			direction.normalize();
			Vector3f force = direction * (G * I.mass * J.mass / distance_squared);
			I.incoming_force += force;
			J.incoming_force -= force;
		}
	}
}

void SimulationFloat::integrate()
{
	// Using Störmer-Verlet, because velocity is lame
	for(Body& i : m_bodies)
	{
		Vector3f acceleration = i.incoming_force * i.inverse_mass;
		i.incoming_force = Vector3f(0.0f, 0.0f, 0.0f);
		Vector3f saved_pos(i.position);
		i.position += i.position - i.previous_position + acceleration * (STEPSIZE * STEPSIZE);
		i.previous_position = saved_pos;
	}
}

void SimulationFloat::handle_collisions()
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
			float distance = (J.position - I.position).length();
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
		Body new_body({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f);
		for(Body* body : ml)
		{
			new_body.mass += body->mass;
		}
		new_body.inverse_mass = 1.0f / new_body.mass;
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

SimulationFloat::Body::Body(Vector3f position, Vector3f previous_position, float mass)
: position(position),
  previous_position(previous_position),
  incoming_force(0.0f, 0.0f, 0.0f),
  mass(mass),
  inverse_mass(mass > 0.0f ? 1.0f/mass : 0.0f), // Don't divide by zero
  radius(radius_from_mass(mass)),
  remove(false)
{
}

float SimulationFloat::radius_from_mass(float mass)
{
	// Assuming homogeneous objects with a density of p kg/m^3 gives
	// volume * p = mass <-> volume = mass/p
	// Sphere volume formula gives
	// volume = (4Pi*r^2)/3 <-> sqrt((3*volume)/(4Pi)) = r
	// Substitution gives
	// sqrt((3*mass/p)/(4Pi)) = r <-> sqrt((3*mass)/(4Pi*p)) = r
	// According to Kokubo & Ida 2012 planetesimals have a density of 2 g/cm^3
	// 2 g/cm^3 = 2000 kg/m^3
	float p = 2000;
	return std::sqrt((3.0f * mass) / (4.0f * PI * p));
}
