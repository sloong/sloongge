#pragma once

#include "SloongMath2.h"
#include "SloongPlane.h"

// DEFINES ////////////////////////////////////////////////////

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
// color mode of polygon
#define PLX_COLOR_MODE_RGB_FLAG     0x8000   // this poly uses RGB color
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000   // this poly uses an indexed 8-bit color 

// double sided flag
#define PLX_2SIDED_FLAG              0x1000   // this poly is double sided
#define PLX_1SIDED_FLAG              0x0000   // this poly is single sided

// shading mode of polygon
#define PLX_SHADE_MODE_PURE_FLAG      0x0000  // this poly is a constant color
#define PLX_SHADE_MODE_CONSTANT_FLAG  0x0000  // alias
#define PLX_SHADE_MODE_FLAT_FLAG      0x2000  // this poly uses flat shading
#define PLX_SHADE_MODE_GOURAUD_FLAG   0x4000  // this poly used gouraud shading
#define PLX_SHADE_MODE_PHONG_FLAG     0x6000  // this poly uses phong shading
#define PLX_SHADE_MODE_FASTPHONG_FLAG 0x6000  // this poly uses phong shading (alias)


// defines for polygons and faces version 1

// attributes of polygons and polygon faces
#define POLY4DV1_ATTR_2SIDED              0x0001
#define POLY4DV1_ATTR_TRANSPARENT         0x0002
#define POLY4DV1_ATTR_8BITCOLOR           0x0004
#define POLY4DV1_ATTR_RGB16               0x0008
#define POLY4DV1_ATTR_RGB24               0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE       0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT   0x0020 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_FLAT       0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG      0x0100
#define POLY4DV1_ATTR_SHADE_MODE_FASTPHONG  0x0100 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_TEXTURE    0x0200 


// states of polygons and faces
#define POLY4DV1_STATE_ACTIVE             0x0001
#define POLY4DV1_STATE_CLIPPED            0x0002
#define POLY4DV1_STATE_BACKFACE           0x0004

// defines for objects version 1
#define OBJECT4DV1_MAX_VERTICES           1024  // 64
#define OBJECT4DV1_MAX_POLYS              1024 // 128

// states for objects
#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

// attributes for objects

// render list defines
#define RENDERLIST4DV1_MAX_POLYS          32768// 16384

// transformation control flags
#define TRANSFORM_LOCAL_ONLY       0  // perform the transformation in place on the
// local/world vertex list 
#define TRANSFORM_TRANS_ONLY       1  // perfrom the transformation in place on the 
// "transformed" vertex list

#define TRANSFORM_LOCAL_TO_TRANS   2  // perform the transformation to the local
// vertex list, but store the results in the
// transformed vertex list

// general culling flags
#define CULL_OBJECT_X_PLANE           0x0001 // cull on the x clipping planes
#define CULL_OBJECT_Y_PLANE           0x0002 // cull on the y clipping planes
#define CULL_OBJECT_Z_PLANE           0x0004 // cull on the z clipping planes
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

// defines for camera rotation sequences
#define CAM_ROT_SEQ_XYZ  0
#define CAM_ROT_SEQ_YXZ  1
#define CAM_ROT_SEQ_XZY  2
#define CAM_ROT_SEQ_YZX  3
#define CAM_ROT_SEQ_ZYX  4
#define CAM_ROT_SEQ_ZXY  5

// defines for special types of camera projections
#define CAM_PROJ_NORMALIZED        0x0001
#define CAM_PROJ_SCREEN            0x0002
#define CAM_PROJ_FOV90             0x0004

#define CAM_MODEL_EULER            0x0008
#define CAM_MODEL_UVN              0x0010

#define UVN_MODE_SIMPLE            0 
#define UVN_MODE_SPHERICAL         1
namespace SoaringLoong
{
	namespace Graphics3D
	{
		class CCamera;
	}
	using Graphics3D::CCamera;
	namespace Loader
	{
		typedef struct POLY4DV1_TYPE
		{
			int state;
			int attr;
			COLORREF color;
			POINT4D_PTR vlist;
			int vert[3];
		}POLY4DV1,*POLY4DV1_PTR;

		typedef struct POLYF4DV1_TYPE
		{
			int state;    // state information
			int attr;     // physical attributes of polygon
			COLORREF color;    // color of polygon
			POINT4D worldPos;

			POINT4D vlist[3];  // the vertices of this triangle
			POINT4D tvlist[3]; // the vertices after transformation if needed

			POLYF4DV1_TYPE *next; // pointer to next polygon in list??
			POLYF4DV1_TYPE *prev; // pointer to previous polygon in list??
		}POLYF4DV1, *POLYF4DV1_PTR;

		// an object based on a vertex list and list of polygons
		typedef struct OBJECT4DV1_TYP
		{
			int  id;           // numeric id of this object
			TCHAR name[64];     // ASCII name of object just for kicks
			int  state;        // state of object
			int  attr;         // attributes of object
			float avg_radius;  // average radius of object used for collision detection
			float max_radius;  // maximum radius of object

			POINT4D world_pos;  // position of object in world

			VECTOR4D dir;       // rotation angles of object in local
			// cords or unit direction vector user defined???

			VECTOR4D ux, uy, uz;  // local axes to track full orientation
			// this is updated automatically during
			// rotation calls

			int num_vertices;   // number of vertices of this object

			POINT4D vlist_local[OBJECT4DV1_MAX_VERTICES]; // array of local vertices
			POINT4D vlist_trans[OBJECT4DV1_MAX_VERTICES]; // array of transformed vertices

			int num_polys;        // number of polygons in object mesh
			POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];  // array of polygons

		} OBJECT4DV1, *OBJECT4DV1_PTR;

		// object to hold the render list, this way we can have more
		// than one render list at a time
		typedef struct RENDERLIST4DV1_TYP
		{
			int state; // state of renderlist ???
			int attr;  // attributes of renderlist ???

			// the render list is an array of pointers each pointing to 
			// a self contained "renderable" polygon face POLYF4DV1
			POLYF4DV1_PTR poly_ptrs[RENDERLIST4DV1_MAX_POLYS];

			// additionally to cut down on allocatation, de-allocation
			// of polygons each frame, here's where the actual polygon
			// faces will be stored
			POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];

			int num_polys; // number of polys in render list

		} RENDERLIST4DV1, *RENDERLIST4DV1_PTR;


		// camera version 1
		typedef struct CAM4DV1_TYP
		{
			int state;      // state of camera
			int attr;       // camera attributes

			POINT4D pos;    // world position of camera used by both camera models

			VECTOR4D dir;   // angles or look at direction of camera for simple 
			// euler camera models, elevation and heading for
			// uvn model

			VECTOR4D u;     // extra vectors to track the camera orientation
			VECTOR4D v;     // for more complex UVN camera model
			VECTOR4D n;

			VECTOR4D target; // look at target

			float view_dist;  // focal length 

			float fov;          // field of view for both horizontal and vertical axes

			// 3d clipping planes
			// if view volume is NOT 90 degree then general 3d clipping
			// must be employed
			float near_clip_z;     // near z=constant clipping plane
			float far_clip_z;      // far z=constant clipping plane

			PLANE3D rt_clip_plane;  // the right clipping plane
			PLANE3D lt_clip_plane;  // the left clipping plane
			PLANE3D tp_clip_plane;  // the top clipping plane
			PLANE3D bt_clip_plane;  // the bottom clipping plane                        

			float viewplane_width;     // width and height of view plane to project onto
			float viewplane_height;    // usually 2x2 for normalized projection or 
			// the exact same size as the viewport or screen window

			// remember screen and viewport are synonomous 
			float viewport_width;     // size of screen/viewport
			float viewport_height;
			float viewport_center_x;  // center of view port (final image destination)
			float viewport_center_y;

			// aspect ratio
			float aspect_ratio;

			// these matrices are not necessarily needed based on the method of
			// transformation, for example, a manual perspective or screen transform
			// and or a concatenated perspective/screen, however, having these 
			// matrices give us more flexibility         

			MATRIX4X4 mcam;   // storage for the world to camera transform matrix
			MATRIX4X4 mper;   // storage for the camera to perspective transform matrix
			MATRIX4X4 mscr;   // storage for the perspective to screen transform matrix

		} CAM4DV1, *CAM4DV1_PTR;


		class CPLGLoader
		{
		public:
			CPLGLoader(CDDraw* pDraw);
			~CPLGLoader(){}

			LPTSTR GetLine( TCHAR* buffer, int szie, FILE* fp);
			HRESULT Load(LPCTSTR strFileName);
			HRESULT LoadOBJECT4DV1(OBJECT4DV1_PTR obj, LPCTSTR strFileName, VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);

			static int Insert_POLYF4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				POLYF4DV1_PTR poly);

			static void Init_CAM4DV1(CAM4DV1_PTR cam,       // the camera object
				int cam_attr,          // attributes
				POINT4D_PTR cam_pos,   // initial camera position
				VECTOR4D_PTR cam_dir,  // initial camera angles
				POINT4D_PTR cam_target, // UVN target
				float near_clip_z,     // near and far clipping planes
				float far_clip_z,
				float fov,             // field of view in degrees
				float viewport_width,  // size of final screen viewport
				float viewport_height);

			static void Reset_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);

			static void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq);

			static void Reset_OBJECT4DV1(OBJECT4DV1_PTR obj);

			static void Build_XYZ_Rotation_MATRIX4X4(float theta_x, // euler angles
				float theta_y,
				float theta_z,
				MATRIX4X4_PTR mrot);

			static void Transform_OBJECT4DV1(OBJECT4DV1_PTR obj, // object to transform
				MATRIX4X4_PTR mt,   // transformation matrix
				int coord_select,   // selects coords to transform
				int transform_basis);

			static int Cull_OBJECT4DV1(OBJECT4DV1_PTR obj,  // object to cull
				CAM4DV1_PTR cam,     // camera to cull relative to
				int cull_flags);

			static void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select);

			static int Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				OBJECT4DV1_PTR obj,
				int insert_local);

			static void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);

			static void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CAM4DV1_PTR cam);

			static void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CMatrix4x4* pMatrix);


			static void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CAM4DV1_PTR cam);
			static void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CCamera* cam);

			static void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CAM4DV1_PTR cam);

			static void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				CCamera* cam);

			static void Draw_RENDERLIST4DV1_Wire16(RENDERLIST4DV1_PTR rend_list,
				UCHAR *video_buffer, int lpitch, RECT window);

			static void Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, // render list to transform
				MATRIX4X4_PTR mt,   // transformation matrix
				int coord_select);

			static void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
				int coord_select);
		};
	}
}