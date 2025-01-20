
#pragma once

// GUCamera is an interface to camera models with 6 degrees-of-freedom (3 positional <x, y, z>, 3 rotational <theta (x), phi (y), psi (z)>)

#include "GUObject.h"
#include "GUVector4.h"
#include "GUMatrix4.h"
#include "GUViewFrustum.h"


namespace CoreStructures {
	
	class GUCamera : public GUObject {

	public:

		// Camera transformation methods
	
		virtual void transformCamera(const GUVector4 &T, const float dTheta, const float dPhi, const float dPsi) = 0; // translate camera by T and rotate the camera by the Euler angles <dTheta, dPhi, dPsi> which are specified in degrees
		
		virtual void transformCamera(const GUVector4 &T) = 0; // translate the camera by T
		
		virtual void transformCamera(const float dTheta, const float dPhi, const float dPsi) = 0; // rotate the camera by <dTheta, dPhi, dPsi> which are specified in degrees


		// Accessor methods

		virtual GUVector4 cameraLocation() const = 0; // return the camera location in world coordinate space

		virtual const GUMatrix4& orientation() const = 0; // return a const reference to the camera's orientation matrix in world coordinate space

		virtual float xRotation() const = 0;
		
		virtual float yRotation() const = 0;

		virtual float zRotation() const = 0;

		virtual const GUViewFrustum& viewFrustum() const = 0; // return a const reference to the camera's view frustum

		virtual const GUMatrix4& viewTransform() const = 0; // return a const reference to the view transform matrix for the camera
		
		virtual const GUMatrix4& inverseViewTransform() const = 0; // return a const reference to the matrix that maps the camera from eye coordinate space to world coordinate space

		virtual const GUMatrix4& projectionTransform() const = 0; // return a const reference the projection transform for the camera.  This is a pass-through method and calls projectionMatrix on the encapsulated GUViewFrustum

		// camera projection setter methods - these act as pass-through methods to the encapsulated GUViewFrustum.  Corresponding reader methods are not specified as these can be accessed via the const interface provided through the GUViewFrustum accessor specified above.  These methods also recalculate the world coordinate planes of the encapsulated frustum

		virtual void setFieldOfView(const float newFovy) = 0; // set fovy to newFovy specified in degrees and recalculate derived frustum values.  newFovy represents the entire vertical field of view

		virtual void setAspectRatio(const float newAspect) = 0; // set aspect (w/h) ratio to newAspect and recalculate derived frustum values

		virtual void setNearPlaneDistance(const float newDistance) = 0; // set the near plane distance to newDistance and recalculate derived frustum values.  newDistance > 0 is assumed

		virtual void setFarPlaneDistance(const float newDistance) = 0; // set the far plane distance to newDistance and recalculate derived frustum values.  If newDistance = 0.0 (as determined by equalf) the encapsulated frustum represents an infinite perspective projection.  newDistance >= 0 is assumed

	};

}
