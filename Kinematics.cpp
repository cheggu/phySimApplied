#include "Kinematics.h"

//where the object 'should' be
vecSpace::Vector2d Kinematics::kinematicsCalculator(Vector2d& position, Vector2d& velocity, Vector2d& acceleration, float simTime)
{
	Vector2d finalPosition;
	Vector2d calcFinalVelocity;
	Vector2d initVelocity = velocity;
	Vector2d initPosition = position;

	//Vfinal = Vstart + a*t
	velocity = initVelocity + (acceleration * simTime);

	//Xfinal = Xstart + ((Vstart + Vfinal)/2) * t
	position = initPosition + (((initVelocity + velocity)*(0.5)) * simTime);

	return position;
}
