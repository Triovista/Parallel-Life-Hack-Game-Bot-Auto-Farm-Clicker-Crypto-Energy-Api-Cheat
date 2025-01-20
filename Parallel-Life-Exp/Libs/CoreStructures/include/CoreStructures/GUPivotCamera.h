
#pragma once

// Model a pivot camera looking at the origin in R3.  The camera by default looks down the negative z axis (using a right-handed coordinate system).  Therefore 'forwards' is along the -z axis.  The camera is actually right/left handed agnostic.  The encapsulated frustum however needs to know the differences for the projection matrix and frustum plane calculations

#include "GUCamera.h"

namespace CoreStructures {

	class GUPivotCamera : public GUCamera {

	private:

		float				theta, phi; // spherical coordinates theta (rotation around the x axis) and phi (rotation around the y axis).  <theta, phi> are stored in degrees.  Zero degree rotation on <theta, phi> places the camera on the +z axis.  A positive phi represents counter-clockwise rotation around the y axis in a right-hand coordinate system.  A positive theta represents a counter-clockwise rotation around the x axis in a right-handed coordinate system

		float				r; // radius of the camera's spherical coordinate model.  This lies in the interval [0, +inf]
		
		GUViewFrustum		F; // view frustum of the camera


		// derived values
		
		GUVector4			C; // camera position
		GUMatrix4			R;  // camera orientation basis derived from <theta, phi>

		GUMatrix4			V; //  view matrix for camera's current position and orientation - maps from world to eye coordinate space
		GUMatrix4			Vinv; // inverse view matrix = V^-1 maps from eye to world coordinate space
		

		//
		// Private API
		//

		void calculateDerivedValues(); // update position, orientation and view matrices when camera rotation and radius is modified

	public:

		// GUPivotCamera constructors

		GUPivotCamera(); // initialise camera parameters so it is placed at the origin looking down the -z axis (for a right-handed camera) or +z axis (for a left-handed camera)

		GUPivotCamera(const float init_theta, const float init_phi, const float init_radius, const float init_fovy, const float init_aspect, const float init_nearPlane, const float init_farPlane=0.0f); // create a camera with orientation <theta, phi> representing Euler angles specified in degrees and Euclidean distance 'init_radius' from the origin.  The frustum / viewplane projection coefficients are defined in init_fovy, specified in degrees spanning the entire vertical field of view angle, init_aspect (w/h ratio), init_nearPlane and init_farPlane.  If init_farPlane = 0.0 (as determined by equalf) then the resulting frustum represents an infinite perspective projection.  This is the default


		// GUPivotCamera interface

		float cameraRadius(); // return the camera radius r

		void scaleCameraRadius(float s); // scale camera radius by s.  s is assumed to lie in the interval (0, +inf].  s < 1.0 reduces the radius while s > 1.0 increases the radius.  This is a wrapper for transformCamera which encapsulates the validation of the radius value

		void incrementCameraRadius(float i); // increment camera radius by i.  The camera radius cannot have a value < 0.0 so the resulting radius lies in the interval [0, +inf].  This is a wrapper for transformCamera which encapsulates the validation of the radius value


		// GUCamera interface

		// Camera transformation methods
	
		void transformCamera(const GUVector4 &T, const float dTheta, const float dPhi, const float dPsi); // increment the radius of the camera's spherical coordinate by T.z and rotate the camera by spherical coordinates (dTheta, dPhi) which are specified in degrees.  A positive dPhi rotates counter-clockwise around the y axis in a right-hand coordinate system.  A positive dTheta rotates counter-clockwise around the x axis in a right-handed coordinate system
		
		void transformCamera(const GUVector4 &T); // increment the radius of the camera's spherical coordinate by T.z
		
		void transformCamera(const float dTheta, const float dPhi, const float dPsi); // rotate the camera by spherical coordinates (dTheta, dPhi) which are specified in degrees.  A positive dPhi rotates counter-clockwise around the y axis in a right-hand coordinate system.  A positive dTheta rotates counter-clockwise around the x axis in a right-handed coordinate system


		// Accessor methods

		GUVector4 cameraLocation() const; // return the camera location in world coordinate space.  The radius of the camera's position in spherical coordinates is the l2 norm of the returned position vector

		const GUMatrix4& orientation() const; // return a const reference to the camera's orientation matrix in world coordinate space

		float xRotation() const; // return the pivot rotation around the x axis (theta) in degrees
		
		float yRotation() const; // return the pivot rotation around the y axis (phi) in degrees

		float zRotation() const; // return 0.0 since the pivot camera does not rotate around the z axis

		const GUViewFrustum& viewFrustum() const; // return a const reference to the camera's view frustum

		const GUMatrix4& viewTransform() const; // return a const reference to the view transform matrix for the camera
		
		const GUMatrix4& inverseViewTransform() const; // return a const reference to the matrix that maps the camera from eye coordinate space to world coordinate space

		const GUMatrix4& projectionTransform() const; // return a const reference the projection transform for the camera.  This is a pass-through method and calls projectionMatrix on the encapsulated GUViewFrustum
		

		// camera projection setter methods - these act as pass-through methods to the encapsulated GUViewFrustum.  Corresponding reader methods are not specified as these can be accessed via the const interface provided through the GUViewFrustum accessor specified above.  These methods also recalculate the world coordinate planes of the encapsulated frustum

		void setFieldOfView(const float newFovy); // set fovy to newFovy specified in degrees and recalculate derived frustum values.  newFovy represents the entire vertical field of view

		void setAspectRatio(const float newAspect); // set aspect (w/h) ratio to newAspect and recalculate derived frustum values

		void setNearPlaneDistance(const float newDistance); // set the near plane distance to newDistance and recalculate derived frustum values.  newDistance > 0 is assumed

		void setFarPlaneDistance(const float newDistance); // set the far plane distance to newDistance and recalculate derived frustum values.  If newDistance = 0.0 (as determined by equalf) the encapsulated frustum represents an infinite perspective projection.  newDistance >= 0 is assumed

	};

}
