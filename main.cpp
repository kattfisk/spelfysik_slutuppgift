#include "Graphics.h"
#include "Simulation.h"
#include "SimulationFloat.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>

namespace
{
	std::string get_settings_value(int index, const SimulationInitialConditions& cond, int steps_per_frame)
	{
		switch(index)
		{
		case 1:
			return std::to_string(cond.step_size);
		case 2:
			return std::to_string(steps_per_frame);
		case 3:
			return std::to_string(cond.random_seed);
		case 4:
			return std::to_string(cond.number_of_bodies);
		case 5:
			return std::to_string(cond.system_mass);
		case 6:
			return std::to_string(cond.mass_variance);
		case 7:
			return std::to_string(cond.distribution);
		case 8:
			return std::to_string(cond.distribution_variance);
		case 9:
			return std::to_string(cond.speed);
		case 0:
			return std::to_string(cond.speed_variance);
		default:
			assert(false && "Incorrect index to get_settings_value");
		}
		return ""; // Silence warning
	}

	void set_settings_value(int index, SimulationInitialConditions& cond, int& steps_per_frame, const std::string& value)
	{
		switch(index)
		{
		case 1:
			cond.step_size = std::stoi(value);
			break;
		case 2:
			steps_per_frame = std::stoi(value);
			break;
		case 3:
			cond.random_seed = std::stoi(value);
			break;
		case 4:
			cond.number_of_bodies = std::stoi(value);
			break;
		case 5:
			cond.system_mass = std::stod(value);
			break;
		case 6:
			cond.mass_variance = std::stod(value);
			break;
		case 7:
			cond.distribution = std::stod(value);
			break;
		case 8:
			cond.distribution_variance = std::stod(value);
			break;
		case 9:
			cond.speed = std::stod(value);
			break;
		case 0:
			cond.speed_variance = std::stod(value);
			break;
		default:
			assert(false && "Incorrect index to set_settings_value");
		}
	}

	std::string get_settings_string(const SimulationInitialConditions& cond, int steps_per_frame)
	{
		std::string string = "Use number keys to change settings \n"
		                     "Press space to start simulation. Press D or F to run performance test using doubles or floats respectively.\n"
				             "Variance is specified as a part of the regular value, e.g. 0.2 = 20% -> +-(0, 10%) \n\n";
		string += "1: Step size = ";
		string += std::to_string(cond.step_size);
		string += " seconds \n";
		string += "2: Steps per frame = ";
		string += std::to_string(steps_per_frame);
		string += " (can be changed at run time)\n";
		string += "3: Random seed = ";
		string += std::to_string(cond.random_seed);
		string += "\n";
		string += "4: Number of bodies = ";
		string += std::to_string(cond.number_of_bodies);
		string += "\n";
		string += "5: System mass = ";
		string += std::to_string(cond.system_mass);
		string += " kg \n";
		string += "6: Mass variance = ";
		string += std::to_string(cond.mass_variance);
		string += "\n";
		string += "7: Distance = ";
		string += std::to_string(cond.distribution);
		string += " meters \n";
		string += "8: Distance variance = ";
		string += std::to_string(cond.distribution_variance);
		string += "\n";
		string += "9: Speed = ";
		string += std::to_string(cond.speed);
		string += " m/s \n";
		string += "0: Speed variance = ";
		string += std::to_string(cond.speed_variance);
		return string;
	}

	void control_camera(Graphics::Camera& camera)
	{
// Move
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			camera.move(Graphics::Camera::Direction::LEFT);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			camera.move(Graphics::Camera::Direction::RIGHT);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			camera.move(Graphics::Camera::Direction::UP);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			camera.move(Graphics::Camera::Direction::DOWN);
		}
// Zoom
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			camera.move(Graphics::Camera::Direction::FORWARD);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			camera.move(Graphics::Camera::Direction::BACK);
		}
// Rotate
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			camera.rotate(Graphics::Camera::Direction::UP);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			camera.rotate(Graphics::Camera::Direction::DOWN);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			camera.rotate(Graphics::Camera::Direction::RIGHT);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			camera.rotate(Graphics::Camera::Direction::LEFT);
		}
	}

	std::string get_time_string(unsigned long long int seconds)
	{
		long long int hours = seconds / (60 * 60);
		int days = hours / 24;
		int years = days / 365;
		hours %= 24;
		days %= 365;

		std::string string =
				std::to_string(years) + " years " + std::to_string(days) + " days " + std::to_string(hours) + " hours ";
		return string;
	}

	// std::to_string does not support scientific notation
	std::string to_scientific_string(double number)
	{
		std::ostringstream strs;
		strs << number;
		return strs.str();
	}

	const std::string CONTROLS_TEXT = "W/S rotate up/down \n"
	                                  "A/D rotate left/right \n"
	                                  "Q/E zoom out/in \n"
			                          "Arrow keys move \n"
			                          "Page up/down change steps \n"
	                                  "Esc quit";
}

int main()
{
	// I apologise for the mess that is this main function. I was in a hurry

	Graphics graphics;

	SimulationInitialConditions cond;
	// These are the default values
	cond.step_size = 60*30;
	int steps_per_frame = 3;
	cond.random_seed = 42;
	cond.number_of_bodies = 128;
	cond.system_mass = 400000000;
	cond.mass_variance = 1.00;
	cond.distribution = 1500;
	cond.distribution_variance = 1.80;
	cond.speed = 0.003;
	cond.speed_variance = 1.8;

	// Whether to run the performance test instead of the interactive program
	enum class PerformanceTest { No, Double, Float } performance_test = PerformanceTest::No;

	// Let user configure values
	{
		bool setup_complete = false;
		int setting_value = -1; // -1 means not setting any value
		std::string user_input = "";
		// Setup loop
		while(graphics.window.isOpen() && !setup_complete)
		{
			// Process events
			sf::Event event;
			while(graphics.window.pollEvent(event))
			{
				// Close graphics.window: exit
				if(event.type == sf::Event::Closed)
				{
					graphics.window.close();
				}
				// Adjust the viewport when the window is resized
				if(event.type == sf::Event::Resized)
				{
					graphics.resize(event.size.width, event.size.height);
				}
				// Escape key exits
				if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				{
					graphics.window.close();
				}
				if(event.type == sf::Event::KeyPressed && setting_value == -1)
				{
					// Spacebar starts simulation
					if(event.key.code == sf::Keyboard::Space)
					{
						setup_complete = true;
					}

					// D starts performance test with doubles
					if(event.key.code == sf::Keyboard::D)
					{
						performance_test = PerformanceTest::Double;
						setup_complete = true;
					}

					// F starts performance test with floats
					if(event.key.code == sf::Keyboard::F)
					{
						performance_test = PerformanceTest::Float;
						setup_complete = true;
					}
				}

				if(event.type == sf::Event::TextEntered)
				{
					if(setting_value == -1)
					{
						if(47 < event.text.unicode && event.text.unicode < 58)
						{
							// If we get a number
							// Select field to edit
							setting_value = static_cast<int>(event.text.unicode - '0');
							user_input = get_settings_value(setting_value, cond, steps_per_frame);
						}
					}
					else
					{
						// Edit field
						if(event.text.unicode == '\n' || event.text.unicode == 13 || event.text.unicode == 10)
						{
							// Handle newline of any kind
							// Done setting value
							set_settings_value(setting_value, cond, steps_per_frame, user_input);
							setting_value = -1;
							user_input = "";
						}
						else if(event.text.unicode == '\b' && user_input.size() > 0)
						{
							// Handle backspace
							user_input.pop_back();
						}
						else if(event.text.unicode == '.' || (47 < event.text.unicode && event.text.unicode < 58))
						{
							// Only accept . and digits
							user_input += static_cast<char>(event.text.unicode);
						}
					}
				}
			}

			// Draw setup process
			graphics.start_frame();
			graphics.set_text_upper(get_settings_string(cond, steps_per_frame));
			graphics.draw_text_upper();
			graphics.set_text_lower(user_input);
			graphics.draw_text_lower();
			graphics.end_frame();
		}
	}

	// Initialize system and values
	Simulation simulation(cond);
	const Vector3d initial_system_velocity = simulation.get_system_velocity();
	unsigned long long int elapsed_sim_time = 0;
	graphics.set_text_upper(CONTROLS_TEXT);
	bool read_input = true;

	// Perform performance test if requested
	if(performance_test != PerformanceTest::No)
	{
		std::string test_type_string = performance_test == PerformanceTest::Double ? "doubles" : "floats";

		// Initialize float system and values
		SimulationFloatInitialConditions cond_float;
		// Copy values over from the double version
		cond_float.step_size = cond.step_size;
		cond_float.random_seed = cond.random_seed;
		cond_float.number_of_bodies = cond.number_of_bodies;
		cond_float.system_mass = cond.system_mass;
		cond_float.mass_variance = cond.mass_variance;
		cond_float.distribution = cond.distribution;
		cond_float.distribution_variance = cond.distribution_variance;
		cond_float.speed = cond.speed;
		cond_float.speed_variance = cond.speed_variance;

		SimulationFloat simulation_float(cond_float);
		Vector3d initial_float_system_velocity = simulation_float.get_system_velocity();

		// Print a message to show while the application locks up (as it's single threaded)
		graphics.start_frame();
		graphics.set_text_upper("Running performance and accuracy test using " + test_type_string + ". \nPlease stand by.");
		graphics.draw_text_upper();
		graphics.end_frame();

		// Run simulations and measure execution time
		// Note that the accuracy is system dependent
		int test_steps = 60000; // Gratuitous large number of steps
		elapsed_sim_time = test_steps * simulation.STEPSIZE;

		auto start_time = std::chrono::steady_clock::now();
		if(performance_test == PerformanceTest::Double)
		{
			simulation.simulate(test_steps);
		}
		else
		{
			simulation_float.simulate(test_steps);
		}
		auto end_time = std::chrono::steady_clock::now();
		auto simulation_run_time = end_time - start_time;

		// Measure velocity deviation
		double deviation;
		{
			Vector3d system_velocity_deviation(0.0, 0.0, 0.0);
			if(performance_test == PerformanceTest::Double)
			{
				system_velocity_deviation = simulation.get_system_velocity() - initial_system_velocity;
			}
			else
			{
				system_velocity_deviation = simulation_float.get_system_velocity() - initial_float_system_velocity;
			}
			deviation = std::abs(system_velocity_deviation.get_x()) + std::abs(system_velocity_deviation.get_y()) + std::abs(system_velocity_deviation.get_z());
		}


		// Loop to show results and wait for user to quit
		while (graphics.window.isOpen())
		{
			// Process events
			sf::Event event;
			while(graphics.window.pollEvent(event))
			{
				// Exit if the close botton on the window is pressed
				if(event.type == sf::Event::Closed)
				{
					graphics.window.close();
				}
				// Exit if the escape key is presed
				if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				{
					graphics.window.close();
				}
				// Adjust the viewport when the window is resized
				if(event.type == sf::Event::Resized)
				{
					graphics.resize(event.size.width, event.size.height);
				}
			}

			int bodies_left = performance_test == PerformanceTest::Double ? simulation.get_body_count() : simulation_float.get_body_count();

			graphics.start_frame();
			graphics.set_text_upper("Results of performance and accuracy test using " + test_type_string);
			graphics.set_text_lower("Run time: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(simulation_run_time).count()) + " ms"
			                        + "\nDeviation in total system velocity: " + to_scientific_string(deviation)
			                        + "\n" + std::to_string(bodies_left) + " bodies remaining after "
			                        + get_time_string(elapsed_sim_time) + " simulated in " + std::to_string(test_steps) + " steps.");
			graphics.draw_text_upper();
			graphics.draw_text_lower();
			graphics.end_frame();
		}
	}


	// Main loop for interactive run
	while (graphics.window.isOpen())
	{
		// Process events
		sf::Event event;
		while (graphics.window.pollEvent(event))
		{
			// Exit if the close botton on the window is pressed
			if(event.type == sf::Event::Closed)
			{
				graphics.window.close();
			}
			// Exit if the escape key is presed
			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				graphics.window.close();
			}
			// Adjust the viewport when the window is resized
			if(event.type == sf::Event::Resized)
			{
				graphics.resize(event.size.width, event.size.height);
			}
			// Only take input if the window has focus
			if(event.type == sf::Event::LostFocus)
			{
				read_input = false;
			}
			if(event.type == sf::Event::GainedFocus)
			{
				read_input = true;
			}
			// Increase/decrease steps using page up/down
			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::PageUp)
				{
					++steps_per_frame;
				}
				else if(event.key.code == sf::Keyboard::PageDown)
				{
					steps_per_frame = std::max(0, steps_per_frame - 1);
				}
			}
		}

		if(read_input)
		{
			control_camera(graphics.camera);
		}

		simulation.simulate(steps_per_frame);
		graphics.start_frame();
		simulation.draw(graphics);

		Vector3d system_velocity_deviation = simulation.get_system_velocity() - initial_system_velocity;
		double deviation = std::abs(system_velocity_deviation.get_x()) + std::abs(system_velocity_deviation.get_y())
						   + std::abs(system_velocity_deviation.get_z());
		elapsed_sim_time += steps_per_frame * simulation.STEPSIZE;
		graphics.set_text_lower("Steps per frame: " + std::to_string(steps_per_frame)
								+ "\nVelocity deviation: " + to_scientific_string(deviation)
								+ "\n" + std::to_string(simulation.get_body_count()) + " bodies, " + get_time_string(elapsed_sim_time));
		graphics.draw_text_lower();
		graphics.draw_text_upper();
		graphics.end_frame();
	}

	return EXIT_SUCCESS;
}
