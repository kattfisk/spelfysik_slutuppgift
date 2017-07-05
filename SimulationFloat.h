#ifndef SPELFYSIK_SLUTUPPGIFT_SIMULATIONFLOAT_H
#define SPELFYSIK_SLUTUPPGIFT_SIMULATIONFLOAT_H

#include "Vector3f.h"
#include "Graphics.h"

#include <deque>

struct SimulationFloatInitialConditions
{
	int step_size;
	int random_seed;
	// No guarantees that it will give exactly this amount of bodies
	int number_of_bodies;
	// In kg
	float system_mass;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	float mass_variance;
	// Minimum distance between particles in meters
	float distribution;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	float distribution_variance;
	// In m/s, directions will be random
	float speed;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	float speed_variance;
};

// Identical to Simulation except it uses floats instead of doubles
class SimulationFloat
{
public:
	SimulationFloat(const SimulationFloatInitialConditions& conditions);
	void simulate(int steps);
	void draw(Graphics& drawer);
	Vector3d get_system_velocity() const;
	int get_body_count() const;

	// In seconds
	const float STEPSIZE;

private:
	void calculate_gravity();
	void integrate();
	void handle_collisions();
	static float radius_from_mass(float mass);

	struct Body
	{
		Body(Vector3f position, Vector3f previous_position, float mass);
		// meters
		Vector3f position;
		Vector3f previous_position;
		// newtons aka kg*m/s^2
		Vector3f incoming_force;
		// kg
		float mass;
		// kg^-1
		float inverse_mass;
		// meters
		float radius;
		bool remove;
	};

	// Deque should give better performance when removing
	// elements and for very large collections
	std::deque<Body> m_bodies;
	// In N*m^2/kg^2
	static const float G;
	static const float PI;
};

#endif //SPELFYSIK_SLUTUPPGIFT_SIMULATIONFLOAT_H
