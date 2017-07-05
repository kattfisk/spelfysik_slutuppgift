#ifndef SPELFYSIK_SLUTUPPGIFT_SIMULATION_H
#define SPELFYSIK_SLUTUPPGIFT_SIMULATION_H

#include "Vector3d.h"
#include "Graphics.h"

#include <deque>

struct SimulationInitialConditions
{
	int step_size;
	int random_seed;
	// No guarantees that it will give exactly this amount of bodies
	int number_of_bodies;
	// In kg
	double system_mass;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	double mass_variance;
	// Minimum distance between particles in meters
	double distribution;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	double distribution_variance;
	// In m/s, directions will be random
	double speed;
	// In percentage, ex. 0.1 = 10% = +-5%, 2.5 = 250% = +-125%
	double speed_variance;
};

class Simulation
{
public:
	Simulation(const SimulationInitialConditions& conditions);
	void simulate(int steps);
	void draw(Graphics& drawer);
	Vector3d get_system_velocity() const;
	int get_body_count() const;

	// In seconds
	const double STEPSIZE;

private:
	void calculate_gravity();
	void integrate();
	void handle_collisions();
	static double radius_from_mass(double mass);

	struct Body
	{
		Body(Vector3d position, Vector3d previous_position, double mass);
		// meters
		Vector3d position;
		Vector3d previous_position;
		// newtons aka kg*m/s^2
		Vector3d incoming_force;
		// kg
		double mass;
		// kg^-1
		double inverse_mass;
		// meters
		double radius;
		bool remove;
	};

	// Deque should give better performance when removing
	// elements and for very large collections
	std::deque<Body> m_bodies;
	// In N*m^2/kg^2
	static const double G;
	static const double PI;
};

#endif //SPELFYSIK_SLUTUPPGIFT_SIMULATION_H
