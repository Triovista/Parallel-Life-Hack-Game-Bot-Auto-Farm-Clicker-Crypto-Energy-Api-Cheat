
#pragma once

// Model first person camera in R3.  The camera by default looks down the negative z axis (using a right-handed coordinate system).  Therefore 'forwards' is along the -z axis.  The camera is actually right/left handed agnostic.  The encapsulated frustum however needs to know the differences for the projection matrix and frustum plane calculations

#include "GUCamera.h"

namespace CoreStructures {

	class GUFirstPersonCamera : public GUCamera {

	private:

		GUVector4			C; // camera position in world coordinte space
		float				theta, phi, psi; // rotation around the x (theta), y (phi) and z (psi) axes representing pitch, yaw and roll respectively.  The rotation angles are stored in degrees
		GUViewFrustum		F; // camera view frustum (describes projection coefficients)

		// derived values
		GUMatrix4			R;  // camera orientation basis derived from <theta, phi, psi>.  This is calculated according to the Roll-Pitch-Yaw convention wrt. global principle axes

		GUMatrix4			V; //  view matrix for camera's current position and orientation - maps from world to eye coordinate space
		GUMatrix4			Vinv; // inverse view matrix = V^-1 maps from eye to world coordinate space


		//
		// Private API
		//

		void calculateOrientationMatrix();  // calculate the local orientation basis matrix R based on the Roll-Pitch-Yaw convention (from global axis pov.)  This produces more correct camera behaviour compared to the GUMatrix4 rotation constructor which rotates in the (global) axis order <x, y, z>.  The internal rotation matrix R is right/left handed agnostic.  It is important to note that the host application uses the -z axis to move 'forward' in a right-handed coordinate system

		void calculateViewMatrix(); // calculate the view (V) and inverse view (Vinv) transformation matrices for the camera's current position and orientation


	public:

		// GUFirstPersonCamera constructors

		GUFirstPersonCamera(); // initialise camera parameters so it is placed at the origin looking down the -z axis (for a right-handed camera) or +z axis (for a left-handed camera)

		GUFirstPersonCamera(const GUVector4 &init_C, const float init_theta, const float init_phi, const float init_psi, const float init_fovy, const float init_aspect, const float init_nearPlane, const float init_farPlane=0.0f); // create a camera at position C in R3 with orientation <theta, phi, psi> representing Euler angles specified in degrees.  The frustum / viewplane projection coefficients are defined in init_fovy, specified in degrees spanning the entire vertical field of view angle, init_aspect (w/h ratio), init_nearPlane and init_farPlane.  If init_farPlane = 0.0 (as determined by equalf) then the resulting frustum represents an infinite perspective projection.  This is the default


		// GUCamera interface


		// Camera transformation methods
	
		void transformCamera(const GUVector4 &T, const float dTheta, const float dPhi, const float dPsi); // translate camera by T and rotate the camera by the Euler angles <dTheta, dPhi, dPsi> which are specified in degrees
		
		void transformCamera(const GUVector4 &T); // translate the camera by T
		
		void transformCamera(const float dTheta, const float dPhi, const float dPsi); // rotate the camera by <dTheta, dPhi, dPsi> which are specified in degrees


		// Accessor methods

		GUVector4 cameraLocation() const; // return the camera location in world coordinate space

		const GUMatrix4& orientation() const; // return a const reference to the camera's orientation matrix in world coordinate space

		float xRotation() const;

		float yRotation() const;
		
		float zRotation() const;

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
