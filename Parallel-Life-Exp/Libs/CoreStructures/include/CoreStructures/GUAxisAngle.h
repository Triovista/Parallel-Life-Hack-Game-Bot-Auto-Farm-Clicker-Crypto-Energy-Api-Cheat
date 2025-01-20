
#pragma once

#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUMatrix4;
	struct GUQuaternion;

	struct GUAxisAngle {

		float		x, y, z; // axis where |<x, y, z>| = 1.0.  If |<x, y, z>| = theta = 0 then this represents no rotation and must be handled accordingly
		float		theta; // angle of rotation modelling in radians
		
		GUAxisAngle(); // default constructor returns a zero rotation vector and theta = 0.0 radians

		GUAxisAngle(float init_x, float init_y, float init_z, float init_theta=0.0f); // create axis angle and normalise axis <x, y, z> to unit length.  init_theta is specified in radians

		GUAxisAngle(const GUAxisAngle& A); // copy constructor

		/*
		// conversion constructors

		GUAxisAngle(const float ex, const float ey, const float ez); // create axis-angle representation from Euler parameters
		GUAxisAngle(const GUMatrix4& R);
		GUAxisAngle(const GUQuaternion& q);

		// mapping functions
		GUMatrix4 matrix() const;
		GUQuaternion quaternion() const;
		*/
	};

}
