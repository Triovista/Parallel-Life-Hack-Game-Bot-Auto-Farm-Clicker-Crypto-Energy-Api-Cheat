//
//  GUMatrix4.h
//  CoreStructures
//
//  GUMatrix4 is used to model transformations in R3.  This is not intended as a generic matrix class, GUMatrix provides this functionality.  GUMatrix4 operates with GUVector4 to provide standard vertex transformation operations.  For compatibility with OpenGL, the matrix is stored in column-major format.  For use in DirectX, the transpose of the matrix can be used.  When pure rotation matrix models are created (basis vectors), the first column models the x axis (tangent), the second column models the y axis (normal) and the third column models the z axis (bi-tangent).  The fourth column is defined as (0, 0, 0, 1) for pure rotation matrices.  Value semantics apply


#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector4;
	struct GUQuaternion;
	struct GUAxisAngle;

	struct GUMatrix4 {

		float		M[16];		// model 4x4 matrix in a column major format
	
	
		// static methods

		static GUMatrix4 identity(); // create and return the identity matrix of order (4 x 4)
	
	
		static GUMatrix4 I(); // create and return the identity matrix.  This calls identity() defined above
	

		static GUMatrix4 rotationMatrix(const float rx, const float ry, const float rz); // create and return the rotation matrix based on Euler angles (rx, ry, rz).  The Euler angles are specified in radians
	
		static GUMatrix4 translationMatrix(const float tx, const float ty, const float tz); // create and return the translation matrix based on offset vector (tx, ty, tz)

		static GUMatrix4 translationMatrix(const GUVector4& T); // create and return the translation matrix based on offset vector (T.x, T.y, T.z)
	
		static GUMatrix4 scaleMatrix(const float sx, const float sy, const float sz); // create and return the scale matrix, scaling on the principle axes <x, y, z> by sx, sy and sx respectively
	
	
		static GUMatrix4 shearMatrix(const GUMatrix4& R, const float s); // create and return a shear matrix to shear by s within the plane defined by the tangent and bi-tangent vectors of the (assumed) orthonormal basis R.  Shearing is performed along the tangent vector
	
	
		static GUMatrix4 perspectiveProjection(const float fov, const float aspect, const float n, const float f);  // create and return a perspective projection matrix defined by the field-of-view (fov), aspect, near plane (n) and far plane (f) parameters.  fov repesents the angle of the vertical field of view in degrees
	
	
		static GUMatrix4 perspectiveProjection(const float l, const float r, const float b, const float t, const float n, const float f);  // create and return a perspective projection matrix using view plane dimension (l, r, b, t), near plane (n) and far plane (f) parameters

	
		static GUMatrix4 infinitePerspectiveProjection(const float fov, const float aspect, const float n);  // create and return a perspective projection matrix defined by the field-of-view (fov), aspect and near plane (n) parameters.  No far plane is specified as the matrix creates a projection with an infinite far plane distance.  fov represents the angle of the vertical field of view in degrees


		static GUMatrix4 orthographicProjection(float l, float r, float b, float t, float n, float f);  // create and return an orthographic projection matrix using view plane dimension (l, r, b, t), near plane (n) and far plane (f) parameters
	
	
		// constructor methods
	
		GUMatrix4(); // default constructor - return the identity matrix	
	
	
		GUMatrix4(float m11, ...);  // define a matrix of order (4 x 4) with data starting from parameter m11.  If (4 x 4) float values are not specified in the parameter list then the behaviour is undefined.  The parameters are specified in row-major format to make it easier to read matrix content in the actual parameter declaration
	
	
		GUMatrix4(const GUVector4& c1, const GUVector4& c2, const GUVector4& c3, const GUVector4& c4); // define a matrix of order (4 x 4) using the parameters <c1, c2, c3, c4> as the column vectors of the matrix
	
	
		GUMatrix4(const GUMatrix4& R); // copy constructor


		// conversion constructors

		GUMatrix4(const GUQuaternion& q); // initialise rotation matrix from quaternion
		GUMatrix4(const GUAxisAngle& A); // initialise rotation matrix from axis angle
	
	
		// accessor methods
	
		float& operator()(unsigned int i, unsigned int j); // non-const element access indexing row i, column j (not zero indexed)	
	
		float operator()(unsigned int i, unsigned int j) const; // const element access indexing row i, column j (not zero indexed)
	
		GUVector4 row(unsigned int i) const;  // return the row vector indexed by row i (not zero-indexed)
	
		GUVector4 column(unsigned int j) const;  // return the column vector indexed by column j (not zero indexed)
	
		GUVector4 xColumnVector() const;  // return the first column vector (x basis vector the matrix forms a basis in R3)
		void setXColumnVector(const GUVector4& v);

		GUVector4 yColumnVector() const;  // return the second column vector (y basis vector the matrix forms a basis in R3)
		void setYColumnVector(const GUVector4& v);

		GUVector4 zColumnVector() const;  // return the third column vector (z basis vector the matrix forms a basis in R3)
		void setZColumnVector(const GUVector4& v);

		GUVector4 wColumnVector() const;  // return the fourth column vector
		void setWColumnVector(const GUVector4& v);

	
	
		// unary operators
	
		GUMatrix4 operator^(const int i) const;  // return the inverse matrix if non-singular, otherwise return the identity matrix.  This operator does nothing unless i = -1.  This method calls inv() and is defined for syntactic convinience only, allowing statements such as Ainv = A^-1 as well as Ainv = A.inv()

	
	
		// binary operators
	
		bool operator==(const GUMatrix4& R) const;  // equality operator - return true if {R(ij)==this(ij) : 1 ≤ (i,j) ≤ 4} where equality is determined by equalf() defined in gu_math.h
	
		GUMatrix4& operator=(const GUMatrix4& R);
	
		GUMatrix4 operator+(const GUMatrix4& A) const; // matrix addition
		GUMatrix4& operator+=(const GUMatrix4& A);

		GUMatrix4 operator-(const GUMatrix4& A) const; // matrix subtraction
		GUMatrix4& operator-=(const GUMatrix4& A);

		GUMatrix4 operator*(const float s) const;  // create and return a new matrix which is the scalar multiplication of *this with scalar s
		GUMatrix4& operator*=(const float s); // scale *this with scalar s

		GUMatrix4 operator*(const GUMatrix4& A) const;  // create and return a new matrix which is the result of post-multiplying A with *this
		GUMatrix4& operator*=(const GUMatrix4& A);  //  modify *this by post-multiplying *this with A
	
		GUVector4 operator*(const GUVector4& v) const;  // create and return a new GUVector4 which is v pre-multiplied with *this
	

	
		// linear algebra methods
	
		GUMatrix4 transpose() const;  // create and return the matrix transpose
	
		GUMatrix4 inverseTranspose() const;  // create and return the inverse transpose of the matrix
	
		float det() const;  // return the matrix determinant
	
		GUMatrix4 inv() const;  // return the inverse matrix if non-singular, otherwise return the identity matrix 
	
		void orthonormalise();  // apply gram-shmidt orthonormalisation.  This orthogonolises the first (x), second (y) then third (z) columns of the matrix.  The fourth column is not processed (the matrix is assumed to form a basis in R3).  This version uses sqrtf() for more precise results

		void _orthonormalise(); // apply gram-shmidt orthonormalisation.  This version uses fastsqrt

	
	
	
		// stream IO functions
	
		friend std::ostream& operator<<(std::ostream& os, const GUMatrix4& A);  // output matrix A to the console in column-major format
	
	};


	// typedef GUMatrix4 mat4;
}
