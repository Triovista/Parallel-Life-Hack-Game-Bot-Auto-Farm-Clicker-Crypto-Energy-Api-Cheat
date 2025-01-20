
#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUDualNumber {
		
		float	a0, ae; // define real (a0) and dual (ae) elements of dual number

		// class methods

		// Create multiplicative identity dual number (1, 0)
		static GUDualNumber I();
		static GUDualNumber identity();


		// Constructors

		GUDualNumber(); // default constructor - return the multiplicative identity (1, 0)
		GUDualNumber(const float real, const float dual);
		GUDualNumber(const GUDualNumber& d); // Copy constructor
		void define(float real, float dual);


		// unary operators

		GUDualNumber operator~() const; // dual number conjugate (a0 - ε ae)
		GUDualNumber conjugate() const;
		GUDualNumber operator^(const int t) const; // d^-1 (multiplicative inverse) iff t=-1.  Dual number inverse is only defined for a0!=0.0
		GUDualNumber operator-(void) const; // negate (additive inverse)
		GUDualNumber sqrt() const; // dual number square root operator - Return sqrt(a0) + ε ae / ( 2 * sqrt(a0) ) where a0 > 0


		// Binary operators

		
		GUDualNumber& operator=(const GUDualNumber& d); // dual number assignment
		bool operator==(const GUDualNumber& d) const; // dual number equality (as determined by equalf)

		GUDualNumber operator+(const GUDualNumber& d) const; // dual number addition
		GUDualNumber& operator+=(const GUDualNumber& d);
		
		GUDualNumber operator-(const GUDualNumber& d) const; // dual number subtraction
		GUDualNumber& operator-=(const GUDualNumber& d);

		GUDualNumber operator*(const GUDualNumber& d) const; // dual number multiplication - post-multiply *this with d
		GUDualNumber& operator*=(const GUDualNumber& d);


#if 0
		// TEST - SCALAR MULTIPLICATION OF DUAL NUMBER
		GUDualNumber& operator*(const float s) const;
#endif


		// stream IO functions
		friend std::ostream& operator<<(std::ostream& os, const GUDualNumber &q);
	};

}