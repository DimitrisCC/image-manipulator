
#pragma once

#include "vec3.h"

using namespace vecmath;

namespace imaging 
{
	// colors are just 3D vectors
	// specialize them to 0..255 integer values per channel (R,G,B)
	
	// create an alias for the 0...255 values (8bit color):
	typedef unsigned char Component;
	
	// use the vec3_t template to define the Color typename:
	typedef vec3_t<Component> Color;
}

	