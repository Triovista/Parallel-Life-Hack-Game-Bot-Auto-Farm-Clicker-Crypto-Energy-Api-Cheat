//
//  GUObject.h
//  CoreStructures
//

#pragma once

#include "GUMemory.h"

namespace CoreStructures {

	// Define core abstract class
	class GUObject {

	private:
		unsigned int			retainCount;

	public:

		GUObject();
	
		// All derived classed have virtual destructors - this ensures appropriate destructor called for based-class pointer referenced sub-classes
		virtual ~GUObject();

		void retain();
		bool release();

		// accessor methods
		unsigned int getRetainCount();
	};
}




