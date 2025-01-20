//
//  GUVector2.h
//  CoreStructures
//
//  GUVector2 models an R2 vector (x, y).  Value semantics apply
//

#pragma once


#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector2 {

		float		x, y;

		// constructors

		GUVector2();
		GUVector2(const float init_x, const float init_y);
		GUVector2(const GUVector2 &V); // copy constructor
		void define(const float _x, const float _y);


		// vector length methods
	
		float l1norm() const;  // L1 norm
	
		float length() const;  // L2 norm

		float normalise();  // normalise the vector to unit length and return the original l2 norm
	
		GUVector2 unitVector() const;  // return the unit length vector leaving the original unchanged.  This is a const version of normalise()
	
		float _length() const;  // fastsqrt version of the above length (L2 norm) method.  The result is less accurate than the length() method but is more efficient
	
		float _normalise();  // fastsqrt version of the above normalise method.  The result is less accurate than the normalise() method but is more efficient
	
		GUVector2 _unitVector() const;  // fastsqrt version of the above unitVector method.  The result is less accurate than the unitVector() method but is more efficient


		// unary operators
	
		GUVector2 operator-(void) const;  // negate vector

		// binary operators

		bool operator==(const GUVector2& v) const;  // return true if the ordered components <x, y> equal <v.x, v.y> as determined by equalf, otherwise return false
	
		bool operator!=(const GUVector2& v) const;

		GUVector2 operator=(const GUVector2& p);
		
		GUVector2 operator+(const GUVector2& p) const;

		GUVector2 &operator+=(const GUVector2& p);
		
		GUVector2 operator-(const GUVector2& p) const;

		GUVector2 &operator-=(const GUVector2& p);

		GUVector2 operator*(const float c) const; // scalar multiplication of GUVector2 by scale factor c

		GUVector2 &operator*=(const float c);

		GUVector2 operator/(const float c) const; // return a new vector that is the given vector divided by c (scaled by 1.0/c)

		GUVector2& operator/=(const float c); // divide vector by c (scale by 1.0/c)

		friend float operator*(const GUVector2 &u, const GUVector2 &v);  // pseudo-cross product / perp-dot product (u^perp . v) where u^perp = (-u.y, u.x).  This is equal to the determinant of matrix with column vectors (u^perp, v) which equals the area of the parallelogram formed by (u, v).  The result is positive if u is clockwise wrt v, negative if u is couterclockwise wrt. v and 0 otherwise

		friend float dotProduct(const GUVector2& p1, const GUVector2& p2);  // inner (dot) product (p1.p2)

		friend GUVector2 orthogonalise(const GUVector2& A, const GUVector2& B);  // return B orthogonalised wrt. A.  This does not normalise the returned vector and the resulting length of the returned vector may not be the same as B.length()

		GUVector2 reflect(const GUVector2& v) const;  // reflect v about *this, which is assumed to be unit length and return the resulting vector


		// stream IO functions
		friend std::ostream& operator<<(std::ostream& os, const GUVector2 &p);
	};

}