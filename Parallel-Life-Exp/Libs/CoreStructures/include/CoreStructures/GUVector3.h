//
//  GUVector3.h
//  CoreStructures
//
//  GUVector3 models an R3 vector (x, y, z).  Value semantics apply
//

#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector2;
	struct GUVector4;
	struct GUMatrix4;

	struct GUVector3 {

		float		x, y, z;
	
		// constructor methods
	
		GUVector3();  // default constructor - initialise GUVector3 to additive identity vector (0.0, 0.0, 0.0)
		GUVector3(float _x, float _y, float _z); // initialise GUVector3 to (_x, _y, _z)
		GUVector3(const GUVector2 &V); // initialise GUVector3 to (V.x, V.y, 0.0)
		GUVector3(const GUVector3 &V); // copy constructor
		void define(float _x, float _y, float _z);  // define vector as (_x, _y, _z)
	
	
		// vector length methods
	
		float l1norm() const;  // L1 norm
	
		float length() const;  // L2 norm

		float normalise();  // normalise the vector to unit length and return the original l2 norm
	
		GUVector3 unitVector() const;  // return the unit length vector leaving the original unchanged.  This is a const version of normalise()
	
		float _length() const;  // fastsqrt version of the above length (L2 norm) method.  The result is less accurate than the length() method but is more efficient
	
		float _normalise();  // fastsqrt version of the above normalise method.  The result is less accurate than the normalise() method but is more efficient
	
		GUVector3 _unitVector() const;  // fastsqrt version of the above unitVector method.  The result is less accurate than the unitVector() method but is more efficient
	
	
		// unary operators
	
		GUVector3 operator-(void) const;  // negate vector
	
		GUMatrix4 operator~() const;  // tidle operator - return a skew-symmetric matrix for the given vector <x, y, z> where |xyz| = size of rotation
	
	
		// binary operators
	
		bool operator==(const GUVector3& v) const;  // return true if the ordered components <x, y, z> equal <v.x, v.y, v.z> as determined by equalf, otherwise return false
	
		bool operator!=(const GUVector3& v) const;
	
		GUVector3& operator=(const GUVector3& v);
	
		GUVector3 operator+(const GUVector3& v) const;  // add v to the given vector resulting in a new vector (x+v.x, y+v.y, z+v.z).  Vector addition is associative and commutative
	
		GUVector3& operator+=(const GUVector3& v);
	
		GUVector3 operator-(const GUVector3& v) const; // return directional vector (this - v)
	
		GUVector3& operator-=(const GUVector3& v); // return directional vector (this - v)
	
		GUVector3 operator*(const float c) const;  // return a new vector that is the given vector scaled by c
	
		GUVector3& operator*=(const float c);  // scale vector by c
	
		GUVector3 operator/(const float c) const; // return a new vector that is the given vector divided by c (scaled by 1.0/c)

		GUVector3& operator/=(const float c); // divide vector by c (scale by 1.0/c)

		friend GUVector3 operator*(const GUVector3 &v1, const GUVector3 &v2);  // cross product (v1 x v2)
	
		friend float dotProduct(const GUVector3& v1, const GUVector3& v2);  // inner (dot) product (v1.v2)
		
		friend GUVector3 orthogonalise(const GUVector3& A, const GUVector3& B);  // return B orthogonalised wrt. A.  This does not normalise the returned vector and the resulting length of the returned vector may not be the same as B.length()

		GUVector3 reflect(const GUVector3& v) const;  // reflect v about *this, which is assumed to be unit length and return the resulting vector
	
		GUVector4 augmentEuclidean(float w) const;  // return the vector <wx, wy, wz, w> in augmented Euclidean space if the parameter w ≠ 0 (as determined by equalf), otherwise return the vector <x, y, z, 0>


		// stream IO functions
		friend std::ostream& operator<<(std::ostream& os, const GUVector3 &v);
	};

}


