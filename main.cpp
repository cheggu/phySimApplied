//----------------------------------------------------IMPORTANT NOTES--------------------------------------------------//
/*
you may see some caution signs or warnings that are NOT ERRORS in the debug menu. Compiler is a beta, you can ignore these unless they screw up the 
code. this is good for now.

The coordinates of an SFML window move accross the screen left to right, and down the screen top to bottom. This means that to the right is positive
and so is down. For example, the coordinate 100, 100 in SFML would 'normaly' be the cartesian coordinate 100, -100. DOWN IS POSITIVE!

Objects are also drawn left to right, top to bottom. This means that a square of length and with 10 would be drawn such that its top left coordinate
is 0,0 but its bottom right is 10,10. To draw the square at a particular location, we have to offset its coordinates.
*/


//Libraries we want to include:
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#include "Kinematics.h"
#include "Vector2d.h"

#define WIDTH 2560
#define HEIGHT 1440 

//window is created.
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Kinematics");

//how often the computer refreshes the window per second
int frameRate = 100;

double timeElapsed = 0;

class Object {
public:
	sf::Shape* drawable;
	float mass;
protected:
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
};

class Rocket : public Object {
private:
	bool grabbable = true;
	bool gravitymouse = false;
	
	void setPos(sf::Vector2f newPos) {
		position = newPos;
	}

public:
	Rocket(sf::Vector2f pos, float m, sf::CircleShape& shape) {
		setPos(pos); setVel({ 0.0f, 0.0f }); setAcc({ 0.0f, 9.81f });
		mass = m;
		drawable = &shape;
	}

	sf::Vector2f getPos() {
		return position;
	}

	sf::Vector2f getVel() {
		return velocity;
	}

	void setAcc(sf::Vector2f a) {
		acceleration = a;
	}

	void setVel(sf::Vector2f v) {
		velocity = v;
	}

	//simulate rocket by one "step", or delta t. 
	//returns new position after one step
	sf::Vector2f simStep(float step) {
		setPos(position + (velocity * step));
		setVel(velocity + (acceleration * step));
		
		updateShape();

		return position;
	}

	//returns new acceleration after applying a force to a mass
	sf::Vector2f applyForce(sf::Vector2f force) {
		setAcc({ force.x / mass, force.y / mass });
		return acceleration;
	}

	//sets the shape's position to whatever is stored in this class
	void updateShape() {
		drawable->setPosition(position);
	}

	void pullTowardsPoint() {
		//arbitrary value we use to slow the acceleration, if this was 1.0f it would teleport to the mouse pos
		float magnitude = 0.1f;

		float mousex = sf::Mouse::getPosition(window).x;
		float mousey = sf::Mouse::getPosition(window).y;

		sf::Vector2f newAccelerationVector;

		//to calculate the acceleration vector, we simply subtract the mouse vector and shape vector and 
		//multiply by our arbitrary magnitude to produce a vector from the shape in the direction of the mouse
		newAccelerationVector = { (mousex - drawable->getPosition().x) * magnitude, ((mousey - drawable->getPosition().y) * magnitude) + 9.81f };

		acceleration = newAccelerationVector;
	}

	void reset() {
		sf::Vector2f resetPos = { (WIDTH / 2.0f) - mass, (HEIGHT / 2.0f) - mass };
		setPos(resetPos);
		setVel({ 0.0f, 0.0f });
		setAcc({ 0.0f, 9.81f });
		drawable->setPosition(position);
	}

	void grab() {
		sf::Vector2f grabPos = { sf::Mouse::getPosition(window).x - (mass), sf::Mouse::getPosition(window).y - (mass) };
		setPos(grabPos);
		setVel({ 0.0f, 0.0f });
	}
};

namespace utils {
	//if the distance between the mouse and the shape is less than the radius of the shape, return true
	//else return false
	bool isGrabbing(sf::CircleShape& shape) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			float xp = sf::Mouse::getPosition(window).x;
			float yp = sf::Mouse::getPosition(window).y;

			float xc = shape.getPosition().x + shape.getRadius();
			float yc = shape.getPosition().y + shape.getRadius();

			//distance formula
			float distance = sqrtf(powf(xp - xc, 2) + powf(yp - yc, 2));

			if (distance < shape.getRadius()) {
				return true;
			}
		}
		return false;
	}

	void startPullEvent(Rocket &rocket) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			//if we are holding the mouse button, accelerate towards the mouse position
			rocket.pullTowardsPoint();

			//setup line vertices to draw between the cursor and the ball
			sf::Vertex vertices[2] =
			{
				sf::Vertex({rocket.drawable->getPosition().x + rocket.mass, rocket.drawable->getPosition().y + rocket.mass}),
				sf::Vertex({(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y})
			};

			//draw the line
			window.draw(vertices, 2, sf::Lines);
		}
		else {
			//reset the acceleration
			rocket.setAcc({ 0, 9.81 });
		}
	}
};

//dt is set at a fraction of the framerate. So instead of updating where it should be an arbitrary number of times, it updates where we should be 
//exactly per frame
float dt = 0.01f;

//size of the ball object
float radius = 20.0f;

sf::Vector2f lastMousePos;
sf::Vector2f curMousePos;
sf::Vector2f deltaMouse;

//-------------------------------------------------MAIN FUNCTION-----------------------------------------------------------//
int main()
{
	auto bulldogRocketryImage = sf::Image{};
	if (!bulldogRocketryImage.loadFromFile("icon.png"))
	{
		// Error handling...
	}

	window.setIcon(bulldogRocketryImage.getSize().x, bulldogRocketryImage.getSize().y, bulldogRocketryImage.getPixelsPtr());

	sf::Texture bulldogRocketryTexture;
	bulldogRocketryTexture.loadFromImage(bulldogRocketryImage);

	sf::Sprite bulldogRocketrySprite;
	bulldogRocketrySprite.setTexture(bulldogRocketryTexture, true);
	bulldogRocketrySprite.setPosition(0, 0);
	bulldogRocketrySprite.setScale({ 0.1f, 0.1f });

	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		// Error handling...
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::Red);
	//text.setPosition(WIDTH - (text.getGlobalBounds().width * 2), 20);
	std::string stats = "";
	text.setString(stats);
	text.setPosition(WIDTH - 220, 20);

	//clock we use to claculate fps and dt
	sf::Clock clock;

	//creating our ball. It is green!
	sf::CircleShape shape(radius);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(0, 0);

	Rocket rocket = Rocket({ 0,0 }, radius, shape);

	//we use this to determine total sim time
	sf::Clock timer;

	bool holding = false;
	bool grabbingAndHolding = false;

	lastMousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

	while (window.isOpen())
	{
		
		//SFML stuff. This block just means that if we press escape or hit the close window button on the window, close the window.
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					window.close();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {				//reset the ball position if it's lost
					rocket.reset();
				}
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				holding = true;
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				if (grabbingAndHolding) {
					if (deltaMouse.x != 0.0f || deltaMouse.y != 0.0f) {
						rocket.setVel((rocket.getVel() + deltaMouse) * 15.0f);
					}
				}
				
				holding = false;
				grabbingAndHolding = false;
			}
		}

		//clean the window, so previous frames are not visible
		window.clear();

		utils::startPullEvent(rocket);

		//reset the clock and calculate fps
		float currentTime = clock.restart().asSeconds();
		dt = currentTime * 10;
		int fps = 1.0f / currentTime;

		rocket.simStep(dt);
		//if we are grabbing the shape, change the position of the ball to the mouse position
		if (utils::isGrabbing(*(sf::CircleShape*)rocket.drawable) && holding || grabbingAndHolding) {
			grabbingAndHolding = true;
			rocket.grab();
			rocket.drawable->setFillColor(sf::Color::Blue);
		}
		else {
			grabbingAndHolding = false;
			rocket.drawable->setFillColor(sf::Color::Green);
		}
																																										//format the debug stats
		stats = "posX: " + std::to_string(rocket.getPos().x) + "\nposY: " + std::to_string(rocket.getPos().y) + "\nfps: " + std::to_string(fps)			
			+ "\nvelocityX: " + std::to_string(rocket.getVel().x) + "\nvelocityY: " + std::to_string(rocket.getVel().y) + "\ntimer: " + std::to_string(timer.getElapsedTime().asSeconds()) + "s"
			+ "\nmousemovedelta: \nX" + std::to_string(deltaMouse.x) + " \nY" + std::to_string(deltaMouse.y);

		text.setString(stats);
		
		curMousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
		if (curMousePos != lastMousePos && holding) {
			deltaMouse = curMousePos - lastMousePos;
			lastMousePos = curMousePos;
		}
		

		window.draw(*rocket.drawable);
		window.draw(bulldogRocketrySprite);
		window.draw(text);

		window.display();
	}

	return 0;

}