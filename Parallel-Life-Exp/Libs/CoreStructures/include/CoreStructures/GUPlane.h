
#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include <iostream>

namespace CoreStructures {

	struct GUVector4;

	struct GUPlane {

	private:
		float			a, b, c, d;

	public:

		GUPlane();
		GUPlane(const GUVector4& v1, const GUVector4& v2, const GUVector4& v3);

		GUVector4 planeNormal();


		// stream IO functions
	
		friend std::ostream& operator<<(std::ostream& os, const GUPlane& P);
	};

}