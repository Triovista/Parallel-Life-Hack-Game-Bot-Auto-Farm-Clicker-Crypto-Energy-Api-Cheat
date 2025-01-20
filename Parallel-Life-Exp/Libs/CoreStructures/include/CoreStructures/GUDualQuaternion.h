
#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include "GUQuaternion.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector4;
	struct GUMatrix4;
	struct GUDualNumber;

	struct GUDualQuaternion {

		GUQuaternion		r; // real quaternion part - represent rotation as a normalised (unit length) quaternion
		GUQuaternion		d; // dual quaternion part - represent position as vector <0, xi, yj, zk> where <x, y, z> represent the position in R3

		// class methods

		static GUDualQuaternion I(); // create multiplicative identity dual quaternion
		static GUDualQuaternion identity();
		static GUDualQuaternion additiveIdentity(); // create additive identity dual quaternion (zero dual quaternion)

		static GUDualQuaternion position(const float x, const float y, const float z); // create pure position dual quaternion where r=1=I, d = <x, y, z>

		static GUDualQuaternion rotation(const float rx, const float ry, const float rz); // create pure rotation r=rotation quaternion, d = 0 from Euler angles (radians) <rx, ry, rz>

		static GUDualQuaternion translation(const float tx, const float ty, const float tz); // create dual quaternion to represent translation T = <tx, ty, tz>.  This calls createPositionDualQuaternion with <tx/2, ty/2, tz/2>

		static GUDualQuaternion screwMotion(const GUVector4& s0, const GUVector4& r, const float theta, const float translate); // model screw motion (see Chasle's theorem) using dual quaternion model (see Kavan 08).  For rotation around an arbitrary axis, translate = 0.0


		// constructors

		GUDualQuaternion(); // default constructor - return multiplicative identity dual quaternion
		GUDualQuaternion(const GUQuaternion& R, const GUQuaternion& D);
		GUDualQuaternion(const GUDualQuaternion& D); // copy constructor

		void define(const GUQuaternion& R, const GUQuaternion& D); // build dual quaternion <R, D>

		GUDualNumber normT1() const; // Return the norm of the dual quaternion q in the form of dual number D = ||q|| = ||q.r|| + ε ( <q.r, q.d> / ||q.r|| ).  This is only defined when ||q.r|| != 0.  This will be true for rigid transformations where ||q.r|| = 1.0

		GUDualNumber normalise(); // normalise dual quaternion q = q / ||q|| and return the original (unnormalised) length as a dual number

		GUDualQuaternion unitDualQuaternion() const; // return normalised dual quaternion q' where q' = q / ||q||


		// unary operators
		GUDualQuaternion operator~() const; // conjugate (reference to which type -> resolve!!!)
		GUDualQuaternion conjugate() const; // type 1 conjugate q† : given q = <r, d>, q† = r† + ε d†
		GUDualQuaternion conjugateT2() const; // type 2 conjugate q† : given q = <r, d>, q† = r - ε d
		GUDualQuaternion conjugateT3() const; // type 3 conjugate q† : given q = <r, d>, q† = r† - ε d†

		GUDualQuaternion operator^(const int i) const; // return the multiplicative inverse of the given dual quaternion iff i=-1.  This calls inv() and is provided for syntactic convinience only allowing statements like qinv = q^-1 as well as qinv = q.inv();
		GUDualQuaternion inv() const;

		GUDualQuaternion operator-() const; // additive inverse q' = -q = (-r, -d)
		GUDualQuaternion& negate();

		// binary operators

		bool operator==(const GUDualQuaternion& q) const; // dual quaternion equality as determined by GUQuaternion equality

		GUDualQuaternion& operator=(const GUDualQuaternion& q); // dual quaternion assignment *this = q

		GUDualQuaternion operator+(const GUDualQuaternion& q) const; // dual quaternion addition q' = Qa + Qb, where Qa = *this and Qb = q
		GUDualQuaternion& operator+=(const GUDualQuaternion &q);

		GUDualQuaternion operator-(const GUDualQuaternion& q) const; // dual quaternion subtraction q' = Qa - Qb, where Qa = *this and Qb = q
		GUDualQuaternion& operator-=(const GUDualQuaternion &q);

		GUDualQuaternion operator*(const float s) const; // scalar multiplication q' = qs, where q = *this
		GUDualQuaternion& operator*=(const float s);

		GUDualQuaternion operator*(const GUDualQuaternion& q) const; // dual quaternion multiplication q' = Qa * Qb = Qa Qb = ra rb + ε(ra db + da rb)  where Qa = *this and Qb = q.  This performs the transformations Qb followed by Qa
		GUDualQuaternion& operator*=(const GUDualQuaternion& q);

		// conversion methods

		GUVector4 translationVector() const; // return the translation vector t where t = 2 r_ d (r_ is conjugate of q->r and d = q->d), where q = *this
		GUMatrix4 matrix() const; // return the matrix representation of the dual quaternion


		// stream IO functions

		friend std::ostream& operator<<(std::ostream& os, const GUDualQuaternion& q);
	};

}



