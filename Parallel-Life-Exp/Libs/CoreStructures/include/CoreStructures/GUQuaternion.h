
#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector4;
	struct GUMatrix4;
	struct GUAxisAngle;

	struct GUQuaternion {

		float		s, i, j, k;

		static GUQuaternion I();
		static GUQuaternion identity();
		static GUQuaternion additiveIdentity();

		GUQuaternion(); // initialise multiplicative identity
		GUQuaternion(const float init_s, const float init_i, const float init_j, const float init_k); // create quaternion from specific parameters
		GUQuaternion(const GUQuaternion& q); // copy constructor

		// conversion constructors

		GUQuaternion(const float rx, const float ry, const float rz); // create quaternion from Euler angles <rx, ry, rz> specified in radians to a quaternion in S3 where |q| = 1.0

		GUQuaternion(const GUMatrix4& R); // create quaternion from matrix
		GUQuaternion(const GUAxisAngle& A); // create quaternion from axis angle
		
		void define(const float s_, const float i_, const float j_, const float k_);

		float length() const; // |q| : Wrapper function for norm.

		float norm() const; // |q| calculated with more precise sqrt() function

		float normF() const; // |q| calculated with fastsqrt() - quicker but less accurate than quaternionNorm()

		GUQuaternion unitQuaternion() const; // return normalised unit length quaternion

		float normalise(); // normalise the given quaternion to unit length and return the original length

		void rotateVector(GUVector4& v) const; // v = qv


		// unary operators

		// conjugate (s, -i, -j, -k)
		GUQuaternion operator~(void) const;

		// multiplicative inverse
		GUQuaternion operator^(const int i) const;  // return the inverse quaternion.  This operator does nothing unless i = -1.  This method calls inv() and is defined for syntactic convinience only, allowing statements such as qinv = q^-1 as well as qinv = q.inv()

		GUQuaternion inv(void) const; // return multiplicative inverse

		// negate (additive inverse)
		GUQuaternion operator-(void) const;
		GUQuaternion& negate(void);

		// binary operators

		bool operator==(const GUQuaternion& q) const;

		GUQuaternion& operator=(const GUQuaternion& q);
	
		GUQuaternion operator+(const GUQuaternion& q) const;
		GUQuaternion& operator+=(const GUQuaternion& q);
	
		GUQuaternion operator-(const GUQuaternion& q) const;
		GUQuaternion& operator-=(const GUQuaternion& q);

		GUQuaternion operator*(const float c) const; // scalar multiply
		GUQuaternion& operator*=(const float c);

		GUQuaternion operator*(const GUQuaternion& q) const; // quaternion multiplication...post-multiply *this by q.  For rotation quaternions, this performs the rotation in q, then the rotation in *this.
		GUQuaternion& operator*=(const GUQuaternion& q);


		// mapping functions

		GUAxisAngle axisAngle() const;  // Map to R4 axis angle representation where |xyz| = 1.0.  It is assumed |q| = 1.0 (ie. q in S3)
		GUMatrix4 matrix() const;

		// symmetric constraint functions
		// (Verify correctness of these!)
		GUQuaternion mapToSymmeticConstraint(const GUQuaternion& qa, const GUQuaternion& qb) const;
		bool isValidOffset(const GUQuaternion& qa, const GUQuaternion& qb) const;

		// stream IO functions
		friend std::ostream& operator<<(std::ostream& os, const GUQuaternion &q);

	};


	// slerp template specialisation for quaternions
	template<>
	GUQuaternion slerp<GUQuaternion>(const GUQuaternion& q0, const GUQuaternion& q1_, const float t);

}

