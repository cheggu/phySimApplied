#pragma once
#include <SFML/Graphics.hpp>
#include "Includes.h"

namespace utils {
	bool drawLinesBetween = false;
	bool freeze = false;
	bool drawDebugStats = true;
	float throwSensitivity = 4.0f;

	void freezeGravity(Rocket& rocket) {
		if (freeze) {
			rocket.reset();
			rocket.setAccelerationConstant({ 0.0f, 0.0f });
		}
		else {
			rocket.reset();
			rocket.setAccelerationConstant({ 0.0f, 9.81f });
		}
	}


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

	void startPullEvent(Rocket& rocket) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			//if we are holding the mouse button, accelerate towards the mouse position
			rocket.pullTowardsPoint();
			drawLinesBetween = true;
		}
		else {
			//reset the acceleration
			if (!utils::freeze) {
				rocket.setAcc({ 0, 9.81 });
			}
			else {
				rocket.setAcc({ 0, 0 });
			}

			drawLinesBetween = false;
		}
	}
};