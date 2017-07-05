#include "Graphics.h"

#include <SFML/OpenGL.hpp>

namespace
{
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const char TITLE[] = "Watch gravity dance";
	// Request a 24-bits depth buffer when creating the window
	const sf::ContextSettings CONTEXT_SETTINGS(24);
}

Graphics::Graphics()
: window(sf::VideoMode(WIDTH, HEIGHT), TITLE, sf::Style::Default, CONTEXT_SETTINGS),
  camera(),
  m_sphere(0),
  m_font(),
  m_text_lower(),
  m_text_upper(),
  m_lightAmb({0.2f, 0.2f, 0.2f, 1.f}),
  m_lightDiff({1.f, 0.9f, 0.8f, 1.f}),
  m_lightSpec({1.f, 1.f, 1.f, 1.f}),
  m_matAmbDiff({0.9f, 0.9f, 0.7f, 1.0f}),
  m_matSpec({0.4f, 0.4f, 0.4f, 1.0f}),
  m_lightPos({-200.f, 100.f, 200.f})
{
	window.setVerticalSyncEnabled(true);

// Set up text
	if (!m_font.loadFromFile("sansation.ttf"))
	{
		assert(false && "Font not loaded.");
	}
	m_text_lower.setFont(m_font);
	m_text_lower.setCharacterSize(25);
	m_text_lower.setColor(sf::Color(255, 255, 255, 170));
	m_text_lower.setPosition(10.f, HEIGHT - 90.f);

	m_text_middle.setFont(m_font);
	m_text_middle.setCharacterSize(20);
	m_text_middle.setColor(sf::Color(255, 255, 255, 170));
	m_text_middle.setPosition(WIDTH / 2.0f, HEIGHT - 400.f);

	m_text_upper.setFont(m_font);
	m_text_upper.setCharacterSize(20);
	m_text_upper.setColor(sf::Color(255, 255, 255, 170));
	m_text_upper.setPosition(10.f, 10.f);

// Make the window the active target for OpenGL calls
	window.setActive();

// Configure viewport and frustum using resize
	resize(window.getSize().x, window.getSize().y);

// Enable Z-buffer and backface culling
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); // Apparently the IcoSphere is inside out

// Set up lighting
	glEnable(GL_NORMALIZE); // Need to normalize since we're using glScale
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

//	GLfloat lightAmb[] = {0.2f, 0.2f, 0.2f, 1.f};
//	GLfloat lightDiff[] = {1.f, 0.9f, 0.8f, 1.f};
//	GLfloat lightSpec[] = {1.f, 1.f, 1.f, 1.f};

	glLightfv(GL_LIGHT1, GL_AMBIENT, m_lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, m_lightDiff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, m_lightSpec);

	glEnable(GL_LIGHT1);

// Set up material
//	GLfloat matAmbDiff[] = {0.9f, 0.9f, 0.7f, 1.0f};
//	GLfloat matSpec[] = {0.4f, 0.4f, 0.4f, 1.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, m_matAmbDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, 40.f);
}

void Graphics::start_frame()
{
	// Clear the depth and color buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Camera movement
	Vector3d camera_pos = camera.get_position();
	glTranslatef(static_cast<float>(-camera_pos.get_x()), static_cast<float>(-camera_pos.get_y()), static_cast<float>(-camera_pos.get_z()));
	glRotatef(camera.get_pitch(), 1.0, 0.0, 0.0);
	glRotatef(camera.get_yaw(), 0.0, 1.0, 0.0);
	// Set light position after camera movement
//	GLfloat lightPos[] = {-200.f, 100.f, 200.f};
	glLightfv(GL_LIGHT1, GL_POSITION, m_lightPos);
}

void Graphics::end_frame()
{
	// Display the rendered frame on screen
	window.display();
}

void Graphics::draw_sphere(const Vector3d& position, double size)
{
	glPushMatrix();

	// Apply transformations
	glTranslated(position.get_x(), position.get_y(), position.get_z());
	glScalef(static_cast<float>(size), static_cast<float>(size), static_cast<float>(size));

	m_sphere.draw(1);

	glPopMatrix();
}

void Graphics::resize(unsigned int width, unsigned int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	GLfloat near_plane = 5.f, far_plane = 300000.f;
	glFrustum(-ratio, ratio, -1.f, 1.f, near_plane, far_plane);
	glMatrixMode(GL_MODELVIEW);
}

void Graphics::draw_text_lower()
{
	window.pushGLStates();
	window.draw(m_text_lower);
	window.popGLStates();
}

void Graphics::set_text_lower(const std::string& string)
{
	m_text_lower.setString(string);
}

void Graphics::draw_text_upper()
{
	window.pushGLStates();
	window.draw(m_text_upper);
	window.popGLStates();
}

void Graphics::set_text_upper(const std::string& string)
{
	m_text_upper.setString(string);
}

Graphics::Camera::Camera()
: m_position(0.0, 0.0, 8000.0),
  m_movement_speed(20.0),
  m_zoom_speed(50.0),
  m_up(0.0, 1.0, 0.0),
  m_right(1.0, 0.0, 0.0),
  m_forward(0.0, 0.0, -1.0),
  m_rotation_speed(0.5f), // Degrees per frame
  m_pitch(0.0f),
  m_yaw(0.0f)
{
}

void Graphics::Camera::move(Graphics::Camera::Direction direction)
{
	switch(direction)
	{
	case Direction::UP:
		m_position += m_up * m_movement_speed;
		break;
	case Direction::DOWN:
		m_position -= m_up * m_movement_speed;
		break;
	case Direction::LEFT:
		m_position -= m_right * m_movement_speed;
		break;
	case Direction::RIGHT:
		m_position += m_right * m_movement_speed;
		break;
	case Direction::FORWARD:
		m_position += m_forward * m_zoom_speed;
		break;
	case Direction::BACK:
		m_position -= m_forward * m_zoom_speed;
		break;
	}
}

Vector3d Graphics::Camera::get_position() const
{
	return m_position;
}

void Graphics::Camera::rotate(Graphics::Camera::Direction direction)
{
	switch(direction)
	{
	case Direction::UP:
		m_pitch += m_rotation_speed;
		break;
	case Direction::DOWN:
		m_pitch -= m_rotation_speed;
		break;
	case Direction::RIGHT:
		m_yaw += m_rotation_speed;
		break;
	case Direction::LEFT:
		m_yaw -= m_rotation_speed;
		break;
	case Direction::FORWARD:
		break;
	case Direction::BACK:
		break;
	}
}

float Graphics::Camera::get_pitch() const
{
	return m_pitch;
}

float Graphics::Camera::get_yaw() const
{
	return m_yaw;
}

