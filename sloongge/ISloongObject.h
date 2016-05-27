#pragma once

// states for objects
#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

// defines for enhanced PLG file format -> PLX
// the surface descriptor is still 16-bit now in the following format
// d15                      d0
//   CSSD | RRRR| GGGG | BBBB

// C is the RGB/indexed color flag
// SS are two bits that define the shading mode
// D is the double sided flag
// and RRRR, GGGG, BBBB are the red, green, blue bits for RGB mode
// or GGGGBBBB is the 8-bit color index for 8-bit mode

// bit masks to simplify testing????
#define PLX_RGB_MASK          0x8000   // mask to extract RGB or indexed color
#define PLX_SHADE_MODE_MASK   0x6000   // mask to extract shading mode
#define PLX_2SIDED_MASK       0x1000   // mask for double sided
#define PLX_COLOR_MASK        0x0fff   // xxxxrrrrggggbbbb, 4-bits per channel RGB
// xxxxxxxxiiiiiiii, indexed mode 8-bit index

// these are the comparision flags after masking

// shading mode of polygon
#define PLX_SHADE_MODE_PURE_FLAG      0x0000  // this poly is a constant color
#define PLX_SHADE_MODE_CONSTANT_FLAG  0x0000  // alias
#define PLX_SHADE_MODE_FLAT_FLAG      0x2000  // this poly uses flat shading
#define PLX_SHADE_MODE_GOURAUD_FLAG   0x4000  // this poly used gouraud shading
#define PLX_SHADE_MODE_PHONG_FLAG     0x6000  // this poly uses phong shading
#define PLX_SHADE_MODE_FASTPHONG_FLAG 0x6000  // this poly uses phong shading (alias)

// double sided flag
#define PLX_2SIDED_FLAG              0x1000   // this poly is double sided
#define PLX_1SIDED_FLAG              0x0000   // this poly is single sided
// color mode of polygon
#define PLX_COLOR_MODE_RGB_FLAG     0x8000   // this poly uses RGB color
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000   // this poly uses an indexed 8-bit color 


#include "math\\SloongVector.h"
using namespace Sloong::Math::Vector;
namespace Sloong
{
	namespace Graphics
	{
		class CDDraw;
	}
}
using namespace Sloong::Graphics;
namespace Sloong
{
	namespace Math
	{
		namespace Matrix
		{
			class CMatrix4x4;
		}
		namespace Polygon
		{
			class IPolygon;
		}
	}
	
	using namespace Math::Matrix;
	using namespace Math::Polygon;
	namespace Graphics3D
	{
		typedef enum _TransformMode
		{
			LocalOnly,
			TransOnly,
			LocalToTrans,
		}TRANS_MODE;

		typedef enum _CullMode
		{
			CULL_ON_X_PLANE = 0x0001, // cull on the x clipping planes
			CULL_ON_Y_PLANE = 0x0002, // cull on the y clipping planes
			CULL_ON_Z_PLANE = 0x0004, // cull on the z clipping planes
			CULL_ON_XYZ_PLANES = (CULL_ON_X_PLANE | CULL_ON_Y_PLANE | CULL_ON_Z_PLANE),
		}CULL_MODE;


	}
}