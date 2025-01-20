
#pragma once

// Model a (symmetric) view frustum in R3.  The frustum model assumes a right-hand coordinate system (OpenGL).  Value semantics apply

#include "GUVector4.h"
#include "GUMatrix4.h"

namespace CoreStructures {

	// enum type to describe the three intersect states with a plane
	enum frustum_plane_intersect	{PL_BACK=0, PL_INTERSECT, PL_FRONT};

	struct GUViewFrustum {

	private:

		// define parameters for the view frustum (projection coefficients)
		float			fovy; // vertical field-of-view specified in degrees - specifies entire field of view, not the half-field of the view
		float			aspect; // aspect ratio (w/h)
		float			flength; // focal length derived from the vertical field-of-view (fovy)
		float			d_near; // distance to the near plane
		float			d_far; // distance to the far plane

		// frustum planes in eye coordinate space.  These only change when the projection coefficients change
		GUVector4		eLeft, eRight, eTop, eBottom, eNear, eFar;

		// projection matrix based on the above projection coefficients
		GUMatrix4		P;


		// Auxiliary values

		// view frustum planes in world coordinates
		GUVector4		wLeft, wRight, wTop, wBottom, wNear, wFar;
		

		//
		// Private API
		//

		void calculateFocalLength(); // given the current projection coefficients, calculate the derived quantity flength (focal length)
		
		void calculateProjectionMatrix(); // calculate the projection matrix for the current projection coefficients.  The matrix is stored in column major format.  An infinite perspective projection matrix is setup if d_far = 0.0 as determined by equalf
		
		void calculateEyeCoordPlanes(); // calculate the frustum planes in eye coordinate space based on the current projection coefficients.  For a frustum representing an infinite perspective projection the far plane coefficients represent a 'placeholder' plane offset 1.0 from the near plane.  This should not be used for view frustum intersection testing and is only setup for testing / frustum visualisation purposes

		//frustum_plane_intersect intersectPlaneAABB(const GUVector4 &plane, const CGAABB &B) const; // Test AABB 'B' against a specific plane within the frustum.  Return PL_INTERSECT if the AABB lies across the plane, PL_FRONT if it lies completely in front of the plane (that is, on the side the plane normal is pointing) and PL_BACK if it lies completely behind the plane


	public:

		GUViewFrustum(); // default constructor - initialise projection coefficients to a basic camera model where fovy=55.0f, aspect = 1.0f, d_near = 0.1f and d_far = 500.0f

		GUViewFrustum(float fovyInit, float aspectInit, float nearInit, float farInit = 0.0f); // initialise the view frustum and set the projection matrix to the infinite projection matrix if farInit = 0.0 as determined by equalf (this is the default)


		// Accessor methods

		const GUMatrix4 &projectionMatrix() const; // return a const reference to the (derived) projection matrix
		
		float verticalFieldOfView() const; // return fovy
		void setFieldOfView(const float newFovy); // set fovy to newFovy specified in degrees and recalculate derived frustum values.  newFovy represents the entire vertical field of view
		
		float aspectRatio() const; // return aspect
		void setAspectRatio(const float newAspect); // set aspect (w/h) ratio to newAspect and recalculate derived frustum values

		float focalLength() const; // return the derived focal length value flength

		float nearPlaneDistance() const; // return d_near
		void setNearPlaneDistance(const float newDistance); // set the near plane distance to newDistance and recalculate derived frustum values.  newDistance > 0 is assumed

		float farPlaneDistance() const; // return d_far (returns 0.0 for an infinite perspective projection)
		void setFarPlaneDistance(const float newDistance); // set the far plane distance to newDistance and recalculate derived frustum values.  If newDistance = 0.0 (as determined by equalf) the frustum represents an infinite perspective projection.  newDistance >= 0 is assumed
		
		bool isInfinite() const; // return true if d_far = 0.0 (as determined by equalf) and the frustum represents an infinite perspective projection, false otherwise

		void getEyeCoordPlanes(GUVector4 *eLeft_, GUVector4 *eRight_, GUVector4 *eTop_, GUVector4 *eBottom_, GUVector4 *eNear_, GUVector4 *eFar_) const; // return the frustum planes in eye coordinates via the specified pointers.  All provided pointers are assumed to be valid

		void getWorldCoordPlanes(GUVector4 *wLeft_, GUVector4 *wRight_, GUVector4 *wTop_, GUVector4 *wBottom_, GUVector4 *wNear_, GUVector4 *wFar_) const; // return the frustum planes in world coordinates via the specified pointers.  All provided pointers are assumed to be valid


		// Frustum plane methods

		void calculateWorldCoordPlanes(const GUVector4 &vcamPos, const GUMatrix4 &R); // Given camera position C and orientation R in R3, calculate the view frustum planes in world coordinate space.  The frustum planes are normalised so |abc| = 1.0 for the plane coefficients <a, b, c, d>

		bool pointInFrustum(const GUVector4 &P) const; // return true if the given point P lies inside the frustum, false otherwise.  P is assumed to be in world coordinate space and it is assumed the world coordinate planes of the frustum have already been calculated with a previous call to CGViewFrustum::calculateWorldCoordPlanes.  If d_far = 0.0 (an infinite perspective projection is used) then the far plane is ignored in determining whether the point lies within the frustum

	};

}
