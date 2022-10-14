//see vector3d for comments

#pragma once

namespace vecSpace
{
	class Vector2d
	{
	public:
		double x;
		double y;

		Vector2d();

		void setXYZ(double a, double b);
		void print(void);
		double magnitude(void);
	};

	const Vector2d operator+(const Vector2d& vector2dLeft, const Vector2d& vector2dRight);

	const Vector2d operator*(const Vector2d& vector2dLeft, const Vector2d& vector2dRight);

	const Vector2d operator*(const Vector2d& vector2dLeft, long double num);

}


