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
protected:
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	float mass;
};

class Rocket : public Object {
private:
	bool grabbable = true;
	bool gravitymouse = false;
	
	void setPos(sf::Vector2f newPos) {
		position = newPos;
	}

	void setVel(sf::Vector2f newVel) {
		velocity = newVel;
	}

	void setAcc(sf::Vector2f newAcc) {
		acceleration = newAcc;
	}

public:
	Rocket(sf::Vector2f pos, float m, sf::CircleShape& shape) {
		setPos(pos); setVel({ 0.0f, 0.0f }); setAcc({ 0.0f, 9.81f });
		mass = m;
		drawable = &shape;
	}

	//simulate rocket by one "step", or delta t. 
	//returns new position after one step
	sf::Vector2f simStep(float step) {
		setPos(position + (velocity * step));
		setVel(velocity + (acceleration * step));

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
};

namespace utils {

};

//dt is set at a fraction of the framerate. So instead of updating where it should be an arbitrary number of times, it updates where we should be 
//exactly per frame
float dt = 0.01f;

//size of the ball object
float radius = 20.0f;

float timeLimit = 2.0f;

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

//modify the acceleration of the shape to point towards the mouse cursor
void pullTowardsMouse(sf::CircleShape& shape, vecSpace::Vector2d& acceleration) {
	//arbitrary value we use to slow the acceleration, if this was 1.0f it would teleport to the mouse pos
	float magnitude = 0.1f;
	
	float mousex = sf::Mouse::getPosition(window).x;
	float mousey = sf::Mouse::getPosition(window).y;

	vecSpace::Vector2d  newAccelerationVector;

	//to calculate the acceleration vector, we simply subtract the mouse vector and shape vector and 
	//multiply by our arbitrary magnitude to produce a vector from the shape in the direction of the mouse
	newAccelerationVector.setXYZ((mousex - shape.getPosition().x) * magnitude, ((mousey - shape.getPosition().y) * magnitude) + 9.81f);

	acceleration = newAccelerationVector;
}

//takes in one circle object and moves it around on the screen. Recall our first few meetings to understand why we multiply by dt
void updateShape(sf::CircleShape& circle, vecSpace::Vector2d& position, vecSpace::Vector2d& velocity, vecSpace::Vector2d& acceleration)
{
	timeElapsed += dt;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//if we are holding the mouse button, accelerate towards the mouse position
		pullTowardsMouse(circle, acceleration);

		//setup line vertices to draw between the cursor and the ball
		sf::Vertex vertices[2] =
		{
			sf::Vertex({circle.getPosition().x + circle.getRadius(), circle.getPosition().y + circle.getRadius()}),
			sf::Vertex({(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y})
		};

		//draw the line
		window.draw(vertices, 2, sf::Lines);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		acceleration.setXYZ(0, acceleration.y - 0.14f);
	}
	else {
		//reset the acceleration
		acceleration.setXYZ(0, 9.81);
	}

	position = position + (velocity * dt);
	velocity = velocity + (acceleration * dt);

	//if it is at the bottom or near the bottom, we should stop so it doesnt continue out of the screen.
	if (position.y < (float)HEIGHT - (radius * 3.14f)) {
		circle.setPosition(position.x + radius, position.y + radius); circle.setFillColor(sf::Color::Green);
	}
	else { 
		circle.setFillColor(sf::Color::Red); 
		velocity.setXYZ(0, 0); 
	}
}

//-------------------------------------------------MAIN FUNCTION-----------------------------------------------------------//
int main()
{
	// ---------- LOAD BULLDOG ROCKETRY IMAGE ---------- //
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

	// ---------- SETUP DEBUG STATS ---------- //
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		// Error handling...
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::Red);
	text.setPosition(WIDTH - (text.getGlobalBounds().width * 2), 20);
	std::string stats = "";
	text.setString(stats);

	//clock we use to claculate fps and dt
	sf::Clock clock;
	float lastTime = 0;

	//creating our ball. It is green!
	sf::CircleShape shape(radius);
	shape.setFillColor(sf::Color::Green);

	//the ball will start at the top left. Remember to offset if you want to move according to its center!
	shape.setPosition(0, 0);

	//initialize vectors
	vecSpace::Vector2d position;
	vecSpace::Vector2d velocity;
	vecSpace::Vector2d acceleration;

	position.setXYZ(shape.getPosition().x, shape.getPosition().y);
	velocity.setXYZ(0, 0);
	acceleration.setXYZ(0, 9.81);

	//we use this to determine total sim time
	sf::Clock timer;

	bool holding = false;
	bool grabbingAndHolding = false;

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
					position.setXYZ((WIDTH / 2.0f) - shape.getRadius(), (HEIGHT / 2.0f) - shape.getRadius());
					velocity.setXYZ(0, 0);
					acceleration.setXYZ(0, 9.81);
					shape.setPosition(position.x, position.y);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				holding = true;
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				holding = false;
				grabbingAndHolding = false;
			}

			else if (event.type == sf::Event::MouseWheelMoved)
			{
				// display number of ticks mouse wheel has moved
				position.y += event.mouseWheel.delta * 10;
			}
		}

		//clean the window, so previous frames are not visible
		window.clear();

		//reset the clock and calculate fps
		float currentTime = clock.restart().asSeconds();
		dt = currentTime * 10;
		int fps = 1.0f / currentTime;
		lastTime = currentTime;

		//update the position of our shape by dt
		updateShape(shape, position, velocity, acceleration);

		//if we are grabbing the shape, change the position of the ball to the mouse position
		if (isGrabbing(shape) && holding || grabbingAndHolding) {
			grabbingAndHolding = true;

			position.setXYZ(sf::Mouse::getPosition(window).x - (shape.getRadius() * 2.0f), sf::Mouse::getPosition(window).y - (shape.getRadius() * 2.0f));
			velocity.setXYZ(0, 0);
			shape.setFillColor(sf::Color::Blue);
		}
		else {
			grabbingAndHolding = false;
		}
		
		//format the debug stats
		stats = "posX: " + std::to_string(position.x) + "\nposY: " + std::to_string(position.y) + "\nfps: " + std::to_string(fps)
			+ "\nvelocityX: " + std::to_string(velocity.x) + "\nvelocityY: " + std::to_string(velocity.y) + "\ntimer: " + std::to_string(timer.getElapsedTime().asSeconds()) + "s";
		text.setString(stats);
		text.setPosition(WIDTH - (text.getGlobalBounds().width) - 20, 20);
		
		//draw our logo
		window.draw(bulldogRocketrySprite);

		//draw the shape!
		window.draw(shape);

		//draw the debug text
		window.draw(text);

		//display drawn elements!
		window.display();
	}

	return 0;

}