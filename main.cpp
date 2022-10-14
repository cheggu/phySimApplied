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
#include "Includes.h"
#include "Object.h"
#include "Rocket.h"
#include "Utils.h"

double timeElapsed = 0;

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
	auto spaceImage = sf::Image{};
	if (!spaceImage.loadFromFile("space.jpg"))
	{
		// Error handling...
	}
	sf::Texture spaceTexure;
	spaceTexure.loadFromImage(spaceImage);

	sf::Sprite spaceSprite;
	spaceSprite.setTexture(spaceTexure, true);
	spaceSprite.setPosition(0, 0);

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
		

		window.draw(spaceSprite);
		if (utils::drawLinesBetween) {
			//setup line vertices to draw between the cursor and the ball
			sf::Vertex vertices[2] =
			{
				sf::Vertex({rocket.drawable->getPosition().x + rocket.mass, rocket.drawable->getPosition().y + rocket.mass}),
				sf::Vertex({(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y})
			};

			//draw the line
			window.draw(vertices, 2, sf::Lines);
		}


		window.draw(*rocket.drawable);
		window.draw(bulldogRocketrySprite);
		window.draw(text);


		window.display();
	}

	return 0;

}