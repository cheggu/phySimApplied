#pragma once
#include <SFML/Graphics.hpp>
#include "Object.h"
#include <iostream>

class rkBody : public Object{
public:
	float h = 0.0025;
	float timeElapsed = 0.0f;

	//functions
	float f_pX(float val) {
		return val + (velocity.x * h);
	}

	float f_pY(float val) {
		return val + (velocity.y * h);
	}

	float f_vX(float val) {
		return val + (acceleration.x * h);
	}

	float f_vY(float val) {
		return val + (acceleration.y * h);
	}

	//actual derivative math calculus shit 
	float rk_posx(float last) {
		float k1, k2, k3, k4;
		float cur;

		k1 = f_pX(last);
		k2 = f_pX(last + h * (k1 / 2.0f));
		k3 = f_pX(last + h * (k2 / 2.0f));
		k4 = f_pX(last + (h * k3));

		cur = last + ((1.0f / 6.0f) * (k1 + (2.0f * k2) + (2.0f * k3) + k4) * h);

		return cur;
	}

	float rk_posy(float last) {
		float k1, k2, k3, k4;
		float cur;

		k1 = f_pY(last);
		k2 = f_pY(last + h * (k1 / 2.0f));
		k3 = f_pY(last + h * (k2 / 2.0f));
		k4 = f_pY(last + (h * k3));

		cur = last + ((1.0f / 6.0f) * (k1 + (2.0f * k2) + (2.0f * k3) + k4) * h);

		return cur;
	}

	float rk_velx(float last) {
		float k1, k2, k3, k4;
		float cur;

		k1 = f_vX(last);
		k2 = f_vX(last + h * (k1 / 2.0f));
		k3 = f_vX(last + h * (k2 / 2.0f));
		k4 = f_vX(last + (h * k3));

		cur = last + ((1.0f / 6.0f) * (k1 + (2.0f * k2) + (2.0f * k3) + k4) * h);

		return cur;
	}

	float rk_vely(float last) {
		float k1, k2, k3, k4;
		float cur;

		k1 = f_vY(last);
		k2 = f_vY(last + (h * (k1 / 2.0f)));
		k3 = f_vY(last + (h * (k2 / 2.0f)));
		k4 = f_vY(last + (h * k3));

		cur = last + ((1.0f / 6.0f) * (k1 + (2.0f * k2) + (2.0f * k3) + k4) * h);

		return cur;
	}

//public:
	//constructors
	rkBody(sf::Vector2f initpos) {
		position = initpos;
		acceleration = { 0.0f, 9.81f };
	}

	rkBody(sf::Vector2f initpos, sf::Vector2f initvel) {
		position = initpos;
		velocity = initvel;
		acceleration = { 0.0f, 9.81f };
	}

	//return next position of the object after time step using ragnakutta shit
	sf::Vector2f rk_next_pos(float dt) {
		h = dt;
		velocity = { rk_velx(velocity.x), rk_vely(velocity.y) };
		position = { rk_posx(position.x), rk_posy(position.y) };

		timeElapsed += h;
		std::cout << "time: " << timeElapsed << std::endl;

		return position;
	}
};




