#ifndef SPELFYSIK_SLUTUPPGIFT_GRAPHICS_H
#define SPELFYSIK_SLUTUPPGIFT_GRAPHICS_H

#include "Vector3d.h"
#include "icosphere.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class Graphics
{
public:
	Graphics();
	void start_frame();
	void end_frame();
	void draw_sphere(const Vector3d& position, double size);
	void resize(unsigned int width, unsigned int height);
	void draw_text_lower();
	void set_text_lower(const std::string& string);
	void draw_text_upper();
	void set_text_upper(const std::string& string);

	class Camera
	{
	public:
		Camera();
		enum class Direction
		{
			UP, DOWN, LEFT, RIGHT, FORWARD, BACK
		};
		void move(Direction direction);
		Vector3d get_position() const;
		void rotate(Direction direction);
		float get_pitch() const;
		float get_yaw() const;

	private:
		Vector3d m_position;
		const double m_movement_speed, m_zoom_speed;
		Vector3d m_up, m_right, m_forward;
		float m_rotation_speed;
		float m_pitch, m_yaw;
	};
	sf::RenderWindow window;
	Camera camera;

private:
	IcoSphere m_sphere;
	sf::Font m_font;
	sf::Text m_text_lower;
	sf::Text m_text_middle;
	sf::Text m_text_upper;
	GLfloat m_lightAmb[4], m_lightDiff[4], m_lightSpec[4];
	GLfloat m_matAmbDiff[4], m_matSpec[4];
	GLfloat m_lightPos[4];
};

#endif //SPELFYSIK_SLUTUPPGIFT_GRAPHICS_H
