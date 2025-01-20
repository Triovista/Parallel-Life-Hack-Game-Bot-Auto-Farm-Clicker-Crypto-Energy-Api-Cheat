//
//  GUVector4.h
//  CoreStructures
//
//  GUVector4 models an R3 vector as a homogeneous coordinate vector (x, y, z, w).  Value semantics apply
//

#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>


namespace CoreStructures {

	struct GUVector2;
	struct GUVector3;
	struct GUMatrix4;

	struct GUVector4 {

		float		x, y, z, w;
	
		// constructor methods
	
		GUVector4();  // default constructor	- initialise GUVector4 to additive identity vector (0.0, 0.0, 0.0, 1.0)
		GUVector4(float _x, float _y, float _z, float _w = 1.0f); // initialise GUVector4 to (_x, _y, _z, _w)
		GUVector4(const GUVector2 &V); // initialise GUVector4 to (V.x, V.y, 0.0, 1.0)
		GUVector4(const GUVector3 &V); // initialise GUVector4 to (V.x, V.y, V.z, 1.0)
		GUVector4(const GUVector4 &V); // copy constructor
		void define(float _x, float _y, float _z, float _w = 1.0f);  // define vector as (_x, _y, _z, _w)
	
	
		// vector length methods
	
		float l1norm() const;  // L1 norm
	
		float length() const;  // L2 norm

		float normalise();  // normalise the vector to unit length and return the original l2 norm
	
		GUVector4 unitVector() const;  // return the unit length vector leaving the original unchanged.  This is a const version of normalise()
	
		float _length() const;  // fastsqrt version of the above length (L2 norm) method.  The result is less accurate than the length() method but is more efficient
	
		float _normalise();  // fastsqrt version of the above normalise method.  The result is less accurate than the normalise() method but is more efficient
	
		GUVector4 _unitVector() const;  // fastsqrt version of the above unitVector method.  The result is less accurate than the unitVector() method but is more efficient
	
	
		// unary operators
	
		GUVector4 operator-(void) const;  // negate vector
	
		GUMatrix4 operator~() const;  // tidle operator - return a skew-symmetric matrix for the given vector <x, y, z> where |xyz| = size of rotation
	
	
		// binary operators
	
		bool operator==(const GUVector4& v) const;  // return true if the ordered components <x, y, z, w> equal <v.x, v.y, v.z, v.w> as determined by equalf, otherwise return false
	
		bool operator!=(const GUVector4& v) const;
	
		GUVector4& operator=(const GUVector4& v);
	
		GUVector4 operator+(const GUVector4& v) const;  // add v to the given vector resulting in a new vector (x+v.x, y+v.y, z+v.z, w).  The w components are not added and the w of the left operand in the expression (ie. the w component of *this) is returned as the w component in the result (which the above expression implies).  Vector addition is associative and commutative
	
		GUVector4& operator+=(const GUVector4& v);
	
		GUVector4 operator-(const GUVector4& v) const; // return directional vector (this - v) where w = 0
	
		GUVector4& operator-=(const GUVector4& v); // return directional vector (this - v) where w = 0
	
		GUVector4 operator*(const float c) const;  // return a new vector that is the given vector scaled by c
	
		GUVector4& operator*=(const float c);  // scale vector by c
	
		GUVector4 operator/(const float c) const; // return a new vector that is the given vector divided by c (scaled by 1.0/c)

		GUVector4& operator/=(const float c); // divide vector by c (scale by 1.0/c)

		friend GUVector4 operator*(const GUVector4 &v1, const GUVector4 &v2);  // cross product (v1 x v2).  Return a directional vector where w = 0.0
	
		friend float dotProduct(const GUVector4& v1, const GUVector4& v2);  // inner (dot) product (v1.v2)
	
		friend GUVector4 orthogonalise(const GUVector4& A, const GUVector4& B);  // return B orthogonalised wrt. A.  This does not normalise the returned vector and the resulting length of the returned vector may not be the same as B.length()

		GUVector4 reflect(const GUVector4& v) const;  // reflect v about *this, which is assumed to be unit length and return the resulting vector
	
		bool isInfinitePoint() const;  // return true if the vector has a homogeneous coordinate of 0 (w==0 as determined by equalf) otherwise return false
	
		bool isDirectionVector() const;  // return true if the vector is a direction vector (representing a point at infinity where inInfinitePoint() returns true), otherwise return false
	
		GUVector4 projectEuclidean() const;  // project homogeneous co-ordinate to Euclidean space if w≠0, returning the vector (x/w, y/w, z/w, 1.0), otherwise return the vector unchanged
	
		GUVector4 augmentEuclidean(float _w) const;  // lift the vector into augmented Euclidean space if the parameter _w ≠ 0 (as determined by equalf), otherwise return the vector unchanged.  It is assumed the vector lies in non-homogeneous space (x,y,z) resulting in the vector (wx, wy, wz, w)
	
	
		// stream IO functions
		friend std::ostream& operator<<(std::ostream& os, const GUVector4 &v);
	};

}


