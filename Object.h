#pragma once
#include <SFML/Graphics.hpp>

class Object {
public:
	sf::Shape* drawable;
	float mass;
protected:
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
};