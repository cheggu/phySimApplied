#pragma once
#include "Includes.h"
#include <SFML/Graphics.hpp>

class Rocket : public Object {
private:
	bool grabbable = true;
	bool gravitymouse = false;

	sf::Vector2f accelerationConstant;

	void setPos(sf::Vector2f newPos) {
		position = newPos;
	}

public:
	Rocket(sf::Vector2f pos, float m, sf::CircleShape& shape) {
		setPos(pos); setVel({ 0.0f, 0.0f }); setAcc({ 0.0f, 9.81f }); accelerationConstant = acceleration;
		mass = m;
		drawable = &shape;
	}

	sf::Vector2f getPos() {
		return position;
	}

	sf::Vector2f getVel() {
		return velocity;
	}

	sf::Vector2f getAcc() {
		return acceleration;
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
		setVel(velocity + (acceleration * step));
		setPos(position + (velocity * step));
		

		updateShape();

		return position;
	}

	//returns new acceleration after applying a force to a mass
	sf::Vector2f applyForce(sf::Vector2f force) {
		setAcc({ acceleration.x + (force.x / mass), acceleration.y + (force.y / mass) });
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

		newAccelerationVector = { 
			(mousex - drawable->getPosition().x) * magnitude,
			((mousey - drawable->getPosition().y) * magnitude) + accelerationConstant.y
		};

		acceleration = newAccelerationVector;
	}

	//sets the default force of acceleration due to gravity, 
	//ie if you want to simulate the forces of gravity in space
	//you would do "rocket.setAccelerationConstant({0.0f, 0.0f});
	void setAccelerationConstant(sf::Vector2f aConst) {
		accelerationConstant = aConst;
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
