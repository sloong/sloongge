#include "stdafx.h"
#include "SloongModelLoader.h"
#include "SloongString.h"
#include "SloongException.h"
#include "IUniversal.h"
#include "SloongCamera.h"
using SoaringLoong::Graphics3D::CCamera;
using SoaringLoong::Universal::CString;
using SoaringLoong::CException;
using namespace SoaringLoong::Loader;
using namespace SoaringLoong::Math;

CDDraw* g_pDDraw;

HRESULT SoaringLoong::Loader::CPLGLoader::Load(LPCTSTR strFileName)
{
	return S_OK;
}


// This function will load one line string from file.
// if the file is empty, return empty string.
LPTSTR SoaringLoong::Loader::CPLGLoader::GetLine(TCHAR* buffer, int size, FILE* fp )
{
	int index = 0;
	int length = 0;

	while (true)
	{
		if (!fgetts(buffer, size, fp))
		{
			return NULL;
		}
	
		for (length = _tcslen(buffer), index = 0; isspace(buffer[index]); index++);

		if ( index >= length || buffer[index] == '#')
		{
			continue;
		}

		return &buffer[index];
	}

}


float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj)
{
	// this function computes the average and maximum radius for 
	// sent object and opdates the object data

	// reset incase there's any residue
	obj->avg_radius = 0;
	obj->max_radius = 0;

	// loop thru and compute radius
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		// update the average and maximum radius
		float dist_to_vertex =
			sqrt(obj->vlist_local[vertex].x*obj->vlist_local[vertex].x +
			obj->vlist_local[vertex].y*obj->vlist_local[vertex].y +
			obj->vlist_local[vertex].z*obj->vlist_local[vertex].z);

		// accumulate total radius
		obj->avg_radius += dist_to_vertex;

		// update maximum radius   
		if (dist_to_vertex > obj->max_radius)
			obj->max_radius = dist_to_vertex;

	} // end for vertex

	// finallize average radius computation
	obj->avg_radius /= obj->num_vertices;

	// return max radius
	return(obj->max_radius);

} // end Compute_OBJECT4DV1_Radius


HRESULT SoaringLoong::Loader::CPLGLoader::LoadOBJECT4DV1(OBJECT4DV1_PTR obj, LPCTSTR strFileName, VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot)
{
	FILE* fp;
	TCHAR buffer[256];  // working buffer

	TCHAR* token_string;  // pointer to actual token text, ready for parsing

	// file format review, note types at end of each description
	// # this is a comment

	// # object descriptor
	// object_name_string num_verts_int num_polys_int

	// # vertex list
	// x0_float y0_float z0_float
	// x1_float y1_float z1_float
	// x2_float y2_float z2_float
	// .
	// .
	// xn_float yn_float zn_float
	//
	// # polygon list
	// surface_description_ushort num_verts_int v0_index_int v1_index_int ..  vn_index_int
	// .
	// .
	// surface_description_ushort num_verts_int v0_index_int v1_index_int ..  vn_index_int

	// lets keep it simple and assume one element per line
	// hence we have to find the object descriptor, read it in, then the
	// vertex list and read it in, and finally the polygon list -- simple :)

	// Step 1: clear out the object and initialize it a bit
	memset(obj, 0, sizeof(OBJECT4DV1));

	// set state of object to active and visible
	obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;

	// set position of object
	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;
	obj->world_pos.w = pos->w;

	CString strTransFileName(strFileName);
	string filename = strTransFileName.GetStringA();
	// Step 2: open the file for reading
	fopen_s(&fp, filename.c_str(), "r");
	if ( fp == NULL )
	{
		strTransFileName.Format(_T("Couldn't open PLG file %s."), strTransFileName.GetString());
		throw CException(strTransFileName.GetString().c_str());
		return(0);
	} // end if

	// Step 3: get the first token string which should be the object descriptor
	token_string = GetLine(buffer,256,fp);
	if (token_string == NULL)
	{
		strTransFileName.Format(_T("PLG file error with file %s (object descriptor invalid)."), strTransFileName.GetString());
		throw CException(strTransFileName.GetString().c_str());
		return(0);
	} // end if

	//Write_Error("Object Descriptor: %s", token_string);

	// parse out the info object
	_stscanf_s(token_string, _T("%s %d %d"), obj->name, 64, &obj->num_vertices, &obj->num_polys);
	
	// Step 4: load the vertex list
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		// get the next vertex
		if (!(token_string = GetLine(buffer, 255, fp)))
		{
			strTransFileName.Format(_T("PLG file error with file %s (vertex list invalid)."), strTransFileName.GetString());
			throw CException(strTransFileName.GetString().c_str());
			return(0);
		} // end if

		// parse out vertex
		stscanf_s(token_string, _T("%f %f %f"), &obj->vlist_local[vertex].x,
			&obj->vlist_local[vertex].y,
			&obj->vlist_local[vertex].z);

		obj->vlist_local[vertex].w = 1;

		// scale vertices
		obj->vlist_local[vertex].x *= scale->x;
		obj->vlist_local[vertex].y *= scale->y;
		obj->vlist_local[vertex].z *= scale->z;

// 		 Write_Error("\nVertex %d = %f, %f, %f, %f", vertex,
// 			obj->vList_local[vertex].x,
// 			obj->vList_local[vertex].y,
// 			obj->vList_local[vertex].z,
// 			obj->vList_local[vertex].w);

	} // end for vertex

	// compute average and max radius
	Compute_OBJECT4DV1_Radius(obj);

// 	Write_Error("\nObject average radius = %f, max radius = %f",
// 		obj->avg_radius, obj->max_radius);

	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts = 0; // number of vertices for current poly (always 3)
	TCHAR tmp_string[8];        // temp string to hold surface descriptor in and
	// test if it need to be converted from hex

	// Step 5: load the polygon list
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// get the next polygon descriptor
		if (!(token_string = GetLine(buffer, 255, fp)))
		{
			strTransFileName.Format(_T("PLG file error with file %s (polygon descriptor invalid)."), strTransFileName.GetString());
			throw CException(strTransFileName.GetString().c_str());
			return(0);
		} // end if

		//Write_Error("\nPolygon %d:", poly);

		// each vertex list MUST have 3 vertices since we made this a rule that all models
		// must be constructed of triangles
		// read in surface descriptor, number of vertices, and vertex list
		stscanf_s(token_string, _T("%s %d %d %d %d"), tmp_string,8,
			&poly_num_verts, // should always be 3 
			&obj->plist[poly].vert[0],
			&obj->plist[poly].vert[1],
			&obj->plist[poly].vert[2]);


		// since we are allowing the surface descriptor to be in hex format
		// with a leading "0x" we need to test for it
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			stscanf_s(tmp_string, _T("%x"), &poly_surface_desc);
		else
			poly_surface_desc = _ttoi(tmp_string);

		// point polygon vertex list to object's vertex list
		// note that this is redundant since the polylist is contained
		// within the object in this case and its up to the user to select
		// whether the local or transformed vertex list is used when building up
		// polygon geometry, might be a better idea to set to NULL in the context
		// of polygons that are part of an object
		obj->plist[poly].vlist = obj->vlist_local;

// 		Write_Error("\nSurface Desc = 0x%.4x, num_verts = %d, vert_indices [%d, %d, %d]",
// 			poly_surface_desc,
// 			poly_num_verts,
// 			obj->plist[poly].vert[0],
// 			obj->plist[poly].vert[1],
// 			obj->plist[poly].vert[2]);

		// now we that we have the vertex list and we have entered the polygon
		// vertex index data into the polygon itself, now let's analyze the surface
		// descriptor and set the fields for the polygon based on the description

		// extract out each field of data from the surface descriptor
		// first let's get the single/double sided stuff out of the way
		if ((poly_surface_desc & PLX_2SIDED_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
			//Write_Error("\n2 sided.");
		} // end if
		else
		{
			// one sided
			//Write_Error("\n1 sided.");
		} // end else

		// now let's set the color type and color
		if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG))
		{
			// this is an RGB 4.4.4 surface
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);

			// now extract color and copy into polygon color
			// field in proper 16-bit format 
			// 0x0RGB is the format, 4 bits per pixel 
			int red = ((poly_surface_desc & 0x0f00) >> 8);
			int green = ((poly_surface_desc & 0x00f0) >> 4);
			int blue = (poly_surface_desc & 0x000f);

			// although the data is always in 4.4.4 format, the graphics card
			// is either 5.5.5 or 5.6.5, but our virtual color system translates
			// 8.8.8 into 5.5.5 or 5.6.5 for us, but we have to first scale all
			// these 4.4.4 values into 8.8.8
			obj->plist[poly].color = RGB(red * 16, green * 16, blue * 16);
			//Write_Error("\nRGB color = [%d, %d, %d]", red, green, blue);
		} // end if
		else
		{
			// this is an 8-bit color indexed surface
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);

			// and simple extract the last 8 bits and that's the color index
			obj->plist[poly].color = (poly_surface_desc & 0x00ff);

			//Write_Error("\n8-bit color index = %d", obj->plist[poly].color);

		} // end else

		// handle shading mode
		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);

		// set polygon shading mode
		switch (shade_mode)
		{
		case PLX_SHADE_MODE_PURE_FLAG: {
										   SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
										  // Write_Error("\nShade mode = pure");
		} break;

		case PLX_SHADE_MODE_FLAT_FLAG: {
										   SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
										   //Write_Error("\nShade mode = flat");

		} break;

		case PLX_SHADE_MODE_GOURAUD_FLAG: {
											  SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
											  //Write_Error("\nShade mode = gouraud");
		} break;

		case PLX_SHADE_MODE_PHONG_FLAG: {
											SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
											//Write_Error("\nShade mode = phong");
		} break;

		default: break;
		} // end switch

		// finally set the polygon to active
		obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;

	} // end for poly

	// close the file
	fclose(fp);

	// return success
	return(1);
}

SoaringLoong::Loader::CPLGLoader::CPLGLoader(CDDraw* pDraw)
{
	g_pDDraw = pDraw;
}

void SoaringLoong::Loader::CPLGLoader::World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CMatrix4x4* pMatrix)
{
	// NOTE: this is a matrix based function
	// 这是一个基于矩阵的函数
	// this function transforms each polygon in the global render list
	// 这个函数转换每一个多边形在全局渲染列表中
	// to camera coordinates based on the sent camera transform matrix
	// 到相机坐标，基于相机转换矩阵
	// you would use this function instead of the object based function
	// 你应该使用这个函数代替这个物体基础函数
	// if you decided earlier in the pipeline to turn each object into 
	// 如果你最初决定在这个官道来转换每一个物体到
	// a list of polygons and then add them to the global render list
	// 一个多边形列表并且把他们添加到全局渲染列表
	// the conversion of an object into polygons probably would have
	// 这个转换对每一个物体很有可能会
	// happened after object culling, local transforms, local to world
	// 发生在物体剪切后，本地转换，本地到世界坐标
	// and backface culling, so the minimum number of polygons from
	//和背面剔除，所以最少的多边形对每一个物体在列表中
	// each object are in the list, note that the function assumes
	// 注意这个函数假设
	// that at LEAST the local to world transform has been called
	// 他是最少的本地到世界转换已经发生过
	// and the polygon data is in the transformed list tvlist of
	// 并且这个多边形数据是已经转换过的
	// the POLYF4DV1 object

	// transform each polygon in the render list into camera coordinates
	// 在渲染列表中转换每一个多边形到相机坐标
	// assumes the render list has already been transformed to world
	// 假设这个渲染列表总是已经转换到世界坐标
	// coordinates and the result is in tvlist[] of each polygon object
	// 并且转换结果存储在tvlist中

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// transform this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concept of "backface" is 
		// irrelevant in a wire frame engine though
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// all good, let's transform 
		for (int vertex = 0; vertex < 3; vertex++)
		{
			// transform point
			curr_poly->tvlist[vertex].Multiply(*pMatrix);
		} // end for vertex

	} // end for poly
}

void SoaringLoong::Loader::CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CCamera* cam)
{
	for (int i = 0; i < rend_list->num_polys; i++ )
	{
		
		for (int v = 0; v < 3;v++)
		{
			rend_list->poly_data[i].tvlist[v].Multiply(cam->MatrixScreen);
			
		}
		for (int j = 0; j < 3; j++)
		{
			rend_list->poly_data[i].tvlist[j].x /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].y /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].z /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].w = 1;
		}
		

	}
}

void SoaringLoong::Loader::CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CCamera* cam)
{
	for (int i = 0; i < rend_list->num_polys; i++)
	{

		for (int v = 0; v < 3; v++)
		{
			rend_list->poly_data[i].tvlist[v].Multiply(cam->MatrixProjection);
		}
		for (int j = 0; j < 3; j++)
		{
			rend_list->poly_data[i].tvlist[j].x /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].y /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].z /= rend_list->poly_data[i].tvlist[j].w;
			rend_list->poly_data[i].tvlist[j].w = 1;
		}


	}
}



void Translate_OBJECT4DV1(OBJECT4DV1_PTR obj, const CVector4D& vt)
{
	// NOTE: Not matrix based
	// this function translates an object without matrices,
	// simply updates the world_pos
	obj->world_pos.Add(vt);

} // end Translate_OBJECT4DV1

/////////////////////////////////////////////////////////////

void Scale_OBJECT4DV1(OBJECT4DV1_PTR obj, VECTOR4D_PTR vs)
{
	// NOTE: Not matrix based
	// this function scales and object without matrices 
	// modifies the object's local vertex list 
	// additionally the radii is updated for the object

	// for each vertex in the mesh scale the local coordinates by
	// vs on a componentwise basis, that is, sx, sy, sz
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		obj->vlist_local[vertex].x *= vs->x;
		obj->vlist_local[vertex].y *= vs->y;
		obj->vlist_local[vertex].z *= vs->z;
		// leave w unchanged, always equal to 1

	} // end for vertex

	// now since the object is scaled we have to do something with 
	// the radii calculation, but we don't know how the scaling
	// factors relate to the original major axis of the object,
	// therefore for scaling factors all ==1 we will simple multiply
	// which is correct, but for scaling factors not equal to 1, we
	// must take the largest scaling factor and use it to scale the
	// radii with since it's the worst case scenario of the new max and
	// average radii

	// find max scaling factor
	float scale = MAX(vs->x, vs->y);
	scale = MAX(scale, vs->z);

	// now scale
	obj->max_radius *= scale;
	obj->avg_radius *= scale;

} // end Scale_OBJECT4DV1

/////////////////////////////////////////////////////////////

void CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(float theta_x, // euler angles
	float theta_y,
	float theta_z,
	MATRIX4X4_PTR mrot) // output 
{
	// this helper function takes a set if euler angles and computes
	// a rotation matrix from them, usefull for object and camera
	// work, also  we will do a little testing in the function to determine
	// the rotations that need to be performed, since there's no
	// reason to perform extra matrix multiplies if the angles are
	// zero!

	MATRIX4X4 mx, my, mz, mtmp;       // working matrices
	float sin_theta = 0, cos_theta = 0;   // used to initialize matrices
	int rot_seq = 0;                  // 1 for x, 2 for y, 4 for z

	// step 0: fill in with identity matrix
	mrot->Zero();

	// step 1: based on zero and non-zero rotation angles, determine
	// rotation sequence
	if (fabs(theta_x) > EPSILON_E5) // x
		rot_seq = rot_seq | 1;

	if (fabs(theta_y) > EPSILON_E5) // y
		rot_seq = rot_seq | 2;

	if (fabs(theta_z) > EPSILON_E5) // z
		rot_seq = rot_seq | 4;

	// now case on sequence
	switch (rot_seq)
	{
	case 0: // no rotation
	{
				// what a waste!
				return;
	} break;

	case 1: // x rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = CMath2::Fast_Cos(theta_x);
				sin_theta = CMath2::Fast_Sin(theta_x);

				// set the matrix up 
				mx.Initialize( 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// that's it, copy to output matrix
				MAT_COPY_4X4(&mx, mrot);
				return;

	} break;

	case 2: // y rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = CMath2::Fast_Cos(theta_y);
				sin_theta = CMath2::Fast_Sin(theta_y);

				// set the matrix up 
				my.Initialize(cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);


				// that's it, copy to output matrix
				MAT_COPY_4X4(&my, mrot);
				return;

	} break;

	case 3: // xy rotation
	{
				// compute the sine and cosine of the angle for x
				cos_theta = CMath2::Fast_Cos(theta_x);
				sin_theta = CMath2::Fast_Sin(theta_x);

				// set the matrix up 
				mx.Initialize( 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle for y
				cos_theta = CMath2::Fast_Cos(theta_y);
				sin_theta = CMath2::Fast_Sin(theta_y);

				// set the matrix up 
				my.Initialize( cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				mrot->Multiply(&mx, &my );
				return;

	} break;

	case 4: // z rotation
	{
				// compute the sine and cosine of the angle
				cos_theta = CMath2::Fast_Cos(theta_z);
				sin_theta = CMath2::Fast_Sin(theta_z);

				// set the matrix up 
				mz.Initialize( cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);


				// that's it, copy to output matrix
				MAT_COPY_4X4(&mz, mrot);
				return;

	} break;

	case 5: // xz rotation
	{
				// compute the sine and cosine of the angle x
				cos_theta = CMath2::Fast_Cos(theta_x);
				sin_theta = CMath2::Fast_Sin(theta_x);

				// set the matrix up 
				mx.Initialize( 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = CMath2::Fast_Cos(theta_z);
				sin_theta = CMath2::Fast_Sin(theta_z);

				// set the matrix up 
				mz.Initialize( cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				mrot->Multiply(&mx, &mz );
				return;

	} break;

	case 6: // yz rotation
	{
				// compute the sine and cosine of the angle y
				cos_theta = CMath2::Fast_Cos(theta_y);
				sin_theta = CMath2::Fast_Sin(theta_y);

				// set the matrix up 
				my.Initialize( cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = CMath2::Fast_Cos(theta_z);
				sin_theta = CMath2::Fast_Sin(theta_z);

				// set the matrix up 
				mz.Initialize( cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices 
				mrot->Multiply(&my, &mz );
				return;

	} break;

	case 7: // xyz rotation
	{
				// compute the sine and cosine of the angle x
				cos_theta = CMath2::Fast_Cos(theta_x);
				sin_theta = CMath2::Fast_Sin(theta_x);

				// set the matrix up 
				mx.Initialize( 1, 0, 0, 0,
					0, cos_theta, sin_theta, 0,
					0, -sin_theta, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle y
				cos_theta = CMath2::Fast_Cos(theta_y);
				sin_theta = CMath2::Fast_Sin(theta_y);

				// set the matrix up 
				my.Initialize( cos_theta, 0, -sin_theta, 0,
					0, 1, 0, 0,
					sin_theta, 0, cos_theta, 0,
					0, 0, 0, 1);

				// compute the sine and cosine of the angle z
				cos_theta = CMath2::Fast_Cos(theta_z);
				sin_theta = CMath2::Fast_Sin(theta_z);

				// set the matrix up 
				mz.Initialize( cos_theta, sin_theta, 0, 0,
					-sin_theta, cos_theta, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

				// concatenate matrices, watch order!
				mtmp.Multiply(&mx, &my);
				mrot->Multiply(&mtmp, &mz);

	} break;

	default: break;

	} // end switch

} // end Build_XYZ_Rotation_MATRIX4X4                                    

///////////////////////////////////////////////////////////

void Build_Model_To_World_MATRIX4X4(VECTOR4D_PTR vpos, MATRIX4X4_PTR m)
{
	// this function builds up a general local to world 
	// transformation matrix that is really nothing more than a translation
	// of the origin by the amount specified in vpos

	m->Initialize( 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vpos->x, vpos->y, vpos->z, 1);

} // end Build_Model_To_World_MATRIX4X4

//////////////////////////////////////////////////////////

void Build_Camera_To_Perspective_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
	// this function builds up a camera to perspective transformation
	// matrix, in most cases the camera would have a 2x2 normalized
	// view plane with a 90 degree FOV, since the point of the having
	// this matrix must be to also have a perspective to screen (viewport)
	// matrix that scales the normalized coordinates, also the matrix
	// assumes that you are working in 4D homogenous coordinates and at 
	// some point there will be a 4D->3D conversion, it might be immediately
	// after this transform is applied to vertices, or after the perspective
	// to screen transform

	m->Initialize( cam->view_dist, 0, 0, 0,
		0, cam->view_dist*cam->aspect_ratio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);

} // end Build_Camera_To_Perspective_MATRIX4X4

///////////////////////////////////////////////////////////

void Build_Perspective_To_Screen_4D_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
	// this function builds up a perspective to screen transformation
	// matrix, the function assumes that you want to perform the
	// transform in homogeneous coordinates and at raster time there will be 
	// a 4D->3D homogenous conversion and of course only the x,y points
	// will be considered for the 2D rendering, thus you would use this
	// function's matrix is your perspective coordinates were still 
	// in homgeneous form whene this matrix was applied, additionally
	// the point of this matrix to to scale and translate the perspective
	// coordinates to screen coordinates, thus the matrix is built up
	// assuming that the perspective coordinates are in normalized form for
	// a (2x2)/aspect_ratio viewplane, that is, x: -1 to 1, y:-1/aspect_ratio to 1/aspect_ratio

	float alpha = (0.5*cam->viewport_width - 0.5);
	float beta = (0.5*cam->viewport_height - 0.5);

	m->Initialize( alpha, 0, 0, 0,
		0, -beta, 0, 0,
		alpha, beta, 1, 0,
		0, 0, 0, 1);

} // end Build_Perspective_To_Screen_4D_MATRIX4X4()

//////////////////////////////////////////////////////////

void Build_Perspective_To_Screen_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
	// this function builds up a perspective to screen transformation
	// matrix, the function assumes that you want to perform the
	// transform in 2D/3D coordinates, that is, you have already converted
	// the perspective coordinates from homogenous 4D to 3D before applying
	// this matrix, additionally
	// the point of this matrix to to scale and translate the perspective
	// coordinates to screen coordinates, thus the matrix is built up
	// assuming that the perspective coordinates are in normalized form for
	// a 2x2 viewplane, that is, x: -1 to 1, y:-1 to 1 
	// the only difference between this function and the version that
	// assumes the coordinates are still in homogenous format is the
	// last column doesn't force w=z, in fact the z, and w results
	// are irrelevent since we assume that BEFORE this matrix is applied
	// all points are already converted from 4D->3D

	float alpha = (0.5*cam->viewport_width - 0.5);
	float beta = (0.5*cam->viewport_height - 0.5);

	m->Initialize( alpha, 0, 0, 0,
		0, -beta, 0, 0,
		alpha, beta, 1, 0,
		0, 0, 0, 1);

} // end Build_Perspective_To_Screen_MATRIX4X4()

///////////////////////////////////////////////////////////

void Build_Camera_To_Screen_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
	// this function creates a single matrix that performs the
	// entire camera->perspective->screen transform, the only
	// important thing is that the camera must be created with
	// a viewplane specified to be the size of the viewport
	// furthermore, after this transform is applied the the vertex
	// must be converted from 4D homogeneous to 3D, technically
	// the z is irrelevant since the data would be used for the
	// screen, but still the division by w is needed no matter
	// what

	float alpha = (0.5*cam->viewport_width - 0.5);
	float beta = (0.5*cam->viewport_height - 0.5);

	m->Initialize( cam->view_dist, 0, 0, 0,
		0, -cam->view_dist, 0, 0,
		alpha, beta, 1, 1,
		0, 0, 0, 0);

} // end Build_Camera_To_Screen_MATRIX4X4()

///////////////////////////////////////////////////////////

void CPLGLoader::Transform_OBJECT4DV1(OBJECT4DV1_PTR obj, // object to transform
	MATRIX4X4_PTR mt,   // transformation matrix
	int coord_select,   // selects coords to transform
	int transform_basis) // flags if vector orientation
	// should be transformed too
{
	// this function simply transforms all of the vertices in the local or trans
	// array by the sent matrix

	// what coordinates should be transformed?
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
								 // transform each local/model vertex of the object mesh in place
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult; // hold result of each transformation

									 // transform point
									 obj->vlist_local[vertex] = mt->Multiply(obj->vlist_local[vertex]);
								 } // end for index
	} break;

	case TRANSFORM_TRANS_ONLY:
	{
								 // transform each "transformed" vertex of the object mesh in place
								 // remember, the idea of the vlist_trans[] array is to accumulate
								 // transformations
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult; // hold result of each transformation

									 // transform point
									 obj->vlist_trans[vertex] = mt->Multiply(obj->vlist_trans[vertex]);

								 } // end for index

	} break;

	case TRANSFORM_LOCAL_TO_TRANS:
	{
									 // transform each local/model vertex of the object mesh and store result
									 // in "transformed" vertex list
									 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
									 {
										 POINT4D presult; // hold result of each transformation

										 // transform point
										 obj->vlist_trans[vertex] = mt->Multiply(obj->vlist_local[vertex]);

									 } // end for index
	} break;

	default: break;

	} // end switch

	// finally, test if transform should be applied to orientation basis
	// hopefully this is a rotation, otherwise the basis will get corrupted
	if (transform_basis)
	{
		// now rotate orientation basis for object
		VECTOR4D vresult; // use to rotate each orientation vector axis

		// rotate ux of basis
		//mt->Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		//vresult.VECTOR4D_COPY(&obj->ux, &vresult);
		obj->ux.Multiply(*mt);

		// rotate uy of basis
		//mt->Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//vresult.VECTOR4D_COPY(&obj->uy, &vresult);
		obj->uy.Multiply(*mt);

		// rotate uz of basis
		//mt->Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
		//vresult.VECTOR4D_COPY(&obj->uz, &vresult);
		obj->uz.Multiply(*mt);
	} // end if

} // end Transform_OBJECT4DV1

///////////////////////////////////////////////////////////

void Rotate_XYZ_OBJECT4DV1(OBJECT4DV1_PTR obj, // object to rotate
	float theta_x,      // euler angles
	float theta_y,
	float theta_z)
{
	// this function rotates and object parallel to the
	// XYZ axes in that order or a subset thereof, without
	// matrices (at least externally sent)
	// modifies the object's local vertex list 
	// additionally it rotates the unit directional vectors
	// that track the objects orientation, also note that each
	// time this function is called it calls the rotation generation
	// function, this is wastefull if a number of object are being rotated
	// by the same matrix, therefore, if that's the case, then generate the
	// rotation matrix, store it, and call the general Transform_OBJECT4DV1()
	// with the matrix

	MATRIX4X4 mrot; // used to store generated rotation matrix

	// generate rotation matrix, no way to avoid rotation with a matrix
	// too much math to do manually!
	CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(theta_x, theta_y, theta_z, &mrot);

	// now simply rotate each point of the mesh in local/model coordinates
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		POINT4D presult; // hold result of each transformation

		// transform point
// 		mrot.Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], &mrot, &presult);
// 
// 		// store result back
// 		presult.VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);
		obj->vlist_local[vertex].Multiply(mrot);

	} // end for index

	// now rotate orientation basis for object
	VECTOR4D vresult; // use to rotate each orientation vector axis

	// rotate ux of basis
// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->ux, &mrot, &vresult);
// 	vresult.VECTOR4D_COPY(&obj->ux, &vresult);
	obj->ux.Multiply(mrot);

	// rotate uy of basis
// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->uy, &mrot, &vresult);
// 	vresult.VECTOR4D_COPY(&obj->uy, &vresult);
	obj->uy.Multiply(mrot);

	// rotate uz of basis
// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->uz, &mrot, &vresult);
// 	vresult.VECTOR4D_COPY(&obj->uz, &vresult);
	obj->uz.Multiply(mrot);

} // end Rotate_XYZ_OBJECT4DV1

////////////////////////////////////////////////////////////

void CPLGLoader::Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select)
{
	// NOTE: Not matrix based
	// this function converts the local model coordinates of the
	// sent object into world coordinates, the results are stored
	// in the transformed vertex list (vlist_trans) within the object

	// interate thru vertex list and transform all the model/local 
	// coords to world coords by translating the vertex list by
	// the amount world_pos and storing the results in vlist_trans[]

	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			// translate vertex
			obj->vlist_trans[vertex] = CVector4D::Add(obj->vlist_local[vertex], obj->world_pos);
		} // end for vertex
	} // end if local
	else
	{ // TRANSFORM_TRANS_ONLY
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			// translate vertex
			obj->vlist_trans[vertex].Add( obj->world_pos );
		} // end for vertex
	} // end else trans

} // end Model_To_World_OBJECT4DV1

////////////////////////////////////////////////////////////

int CPLGLoader::Cull_OBJECT4DV1(OBJECT4DV1_PTR obj,  // object to cull
	CAM4DV1_PTR cam,     // camera to cull relative to
	int cull_flags)     // clipping planes to consider
{
	// NOTE: is matrix based
	// this function culls an entire object from the viewing
	// frustrum by using the sent camera information and object
	// the cull_flags determine what axes culling should take place
	// x, y, z or all which is controlled by ORing the flags
	// together
	// if the object is culled its state is modified thats all
	// this function assumes that both the camera and the object
	// are valid!

	// step 1: transform the center of the object's bounding
	// sphere into camera space

	POINT4D sphere_pos; // hold result of transforming center of bounding sphere

	// transform point
	sphere_pos = cam->mcam.Multiply(obj->world_pos);

	// step 2:  based on culling flags remove the object
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		// cull only based on z clipping planes

		// test far plane
		if (((sphere_pos.z - obj->max_radius) > cam->far_clip_z) ||
			((sphere_pos.z + obj->max_radius) < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return(1);
		} // end if

	} // end if

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		// cull only based on x clipping planes
		// we could use plane equations, but simple similar triangles
		// is easier since this is really a 2D problem
		// if the view volume is 90 degrees the the problem is trivial
		// buts lets assume its not

		// test the the right and left clipping planes against the leftmost and rightmost
		// points of the bounding sphere
		float z_test = (0.5)*cam->viewplane_width*sphere_pos.z / cam->view_dist;

		if (((sphere_pos.x - obj->max_radius) > z_test) || // right side
			((sphere_pos.x + obj->max_radius) < -z_test))  // left side, note sign change
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return(1);
		} // end if
	} // end if

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		// cull only based on y clipping planes
		// we could use plane equations, but simple similar triangles
		// is easier since this is really a 2D problem
		// if the view volume is 90 degrees the the problem is trivial
		// buts lets assume its not

		// test the the top and bottom clipping planes against the bottommost and topmost
		// points of the bounding sphere
		float z_test = (0.5)*cam->viewplane_height*sphere_pos.z / cam->view_dist;

		if (((sphere_pos.y - obj->max_radius) > z_test) || // top side
			((sphere_pos.y + obj->max_radius) < -z_test))  // bottom side, note sign change
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return(1);
		} // end if

	} // end if

	// return failure to cull
	return(0);

} // end Cull_OBJECT4DV1

////////////////////////////////////////////////////////////

void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function removes the backfaces from an object's
	// polygon mesh, the function does this based on the vertex
	// data in vlist_trans along with the camera position (only)
	// note that only the backface state is set in each polygon

	// test if the object is culled
	if (obj->state & OBJECT4DV1_STATE_CULLED)
		return;

	// process each poly in mesh
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1_PTR curr_poly = &obj->plist[poly];

		// is this polygon valid?
		// test this polygon if and only if it's not clipped, not culled,
		// active, and visible and not 2 sided. Note we test for backface in the event that
		// a previous call might have already determined this, so why work
		// harder!
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->attr  & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];

		// we will use the transformed polygon vertex list since the backface removal
		// only makes sense at the world coord stage further of the pipeline 

		// we need to compute the normal of this polygon face, and recall
		// that the vertices are in cw order, u = p0->p1, v=p0->p2, n=uxv
		VECTOR4D u, v, n;

		// build u, v
		u.VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
		v.VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

		// compute cross product
		n = CVector4D::Cross(u, v);

		// now create eye vector to viewpoint
		VECTOR4D view;
		view.VECTOR4D_Build(&obj->vlist_trans[vindex_0], &cam->pos, &view);

		// and finally, compute the dot product
		float dp = n.Dot(view);

		// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
		if (dp <= 0.0)
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);

	} // end for poly

} // end Remove_Backfaces_OBJECT4DV1

////////////////////////////////////////////////////////////

void CPLGLoader::Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function removes the backfaces from polygon list
	// the function does this based on the polygon list data
	// tvlist along with the camera position (only)
	// note that only the backface state is set in each polygon

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// test this polygon if and only if it's not clipped, not culled,
		// active, and visible and not 2 sided. Note we test for backface in the event that
		// a previous call might have already determined this, so why work
		// harder!
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->attr  & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// we need to compute the normal of this polygon face, and recall
		// that the vertices are in cw order, u = p0->p1, v=p0->p2, n=uxv
		VECTOR4D u, v, n;

		// build u, v
		u.VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
		v.VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);

		// compute cross product
		n = CVector4D::Cross(u, v);

		// now create eye vector to viewpoint
		VECTOR4D view;
		view.VECTOR4D_Build(&curr_poly->tvlist[0], &cam->pos, &view);

		// and finally, compute the dot product
		float dp = n.Dot(view);

		// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
		if (dp <= 0.0)
			SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);

	} // end for poly

} // end Remove_Backfaces_RENDERLIST4DV1

////////////////////////////////////////////////////////////

void World_To_Camera_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	// NOTE: this is a matrix based function
	// this function transforms the world coordinates of an object
	// into camera coordinates, based on the sent camera matrix
	// but it totally disregards the polygons themselves,
	// it only works on the vertices in the vlist_trans[] list
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)

	// transform each vertex in the object to camera coordinates
	// assumes the object has already been transformed to world
	// coordinates and the result is in vlist_trans[]
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		// transform the vertex by the mcam matrix within the camera
		// it better be valid!
		POINT4D presult; // hold result of each transformation

		// transform point
// 		cam->mcam.Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], &cam->mcam, &presult);
// 
// 		// store result back
// 		presult.VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
		obj->vlist_trans[vertex].Multiply(cam->mcam);
	} // end for vertex

} // end World_To_Camera_OBJECT4DV1

////////////////////////////////////////////////////////////

void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the camera coordinates of an object
	// into perspective coordinates, based on the 
	// sent camera object, but it totally disregards the polygons themselves,
	// it only works on the vertices in the vlist_trans[] list
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!

	// transform each vertex in the object to perspective coordinates
	// assumes the object has already been transformed to camera
	// coordinates and the result is in vlist_trans[]
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;

		// transform the vertex by the view parameters in the camera
		obj->vlist_trans[vertex].x = cam->view_dist*obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist*obj->vlist_trans[vertex].y*cam->aspect_ratio / z;
		// z = z, so no change

		// not that we are NOT dividing by the homogenous w coordinate since
		// we are not using a matrix operation for this version of the function 

	} // end for vertex

} // end Camera_To_Perspective_OBJECT4DV1

//////////////////////////////////////////////////////////////

void Camera_To_Perspective_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the camera coordinates of an object
	// into Screen scaled perspective coordinates, based on the 
	// sent camera object, that is, view_dist_h and view_dist_v 
	// should be set to cause the desired (width X height)
	// projection of the vertices, but the function totally 
	// disregards the polygons themselves,
	// it only works on the vertices in the vlist_trans[] list
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!
	// finally, the function also inverts the y axis, so the coordinates
	// generated from this function ARE screen coordinates and ready for
	// rendering

	float alpha = (0.5*cam->viewport_width - 0.5);
	float beta = (0.5*cam->viewport_height - 0.5);

	// transform each vertex in the object to perspective screen coordinates
	// assumes the object has already been transformed to camera
	// coordinates and the result is in vlist_trans[]
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;

		// transform the vertex by the view parameters in the camera
		obj->vlist_trans[vertex].x = cam->view_dist*obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist*obj->vlist_trans[vertex].y / z;
		// z = z, so no change

		// not that we are NOT dividing by the homogenous w coordinate since
		// we are not using a matrix operation for this version of the function 

		// now the coordinates are in the range x:(-viewport_width/2 to viewport_width/2)
		// and y:(-viewport_height/2 to viewport_height/2), thus we need a translation and
		// since the y-axis is inverted, we need to invert y to complete the screen 
		// transform:
		obj->vlist_trans[vertex].x = obj->vlist_trans[vertex].x + alpha;
		obj->vlist_trans[vertex].y = -obj->vlist_trans[vertex].y + beta;

	} // end for vertex

} // end Camera_To_Perspective_Screen_OBJECT4DV1

//////////////////////////////////////////////////////////////

void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the perspective coordinates of an object
	// into screen coordinates, based on the sent viewport info
	// but it totally disregards the polygons themselves,
	// it only works on the vertices in the vlist_trans[] list
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!
	// this function would be called after a perspective
	// projection was performed on the object

	// transform each vertex in the object to screen coordinates
	// assumes the object has already been transformed to perspective
	// coordinates and the result is in vlist_trans[]

	float alpha = (0.5*cam->viewport_width - 0.5);
	float beta = (0.5*cam->viewport_height - 0.5);

	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		// assumes the vertex is in perspective normalized coords from -1 to 1
		// on each axis, simple scale them to viewport and invert y axis and project
		// to screen

		// transform the vertex by the view parameters in the camera
		obj->vlist_trans[vertex].x = alpha + alpha*obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta *obj->vlist_trans[vertex].y;

	} // end for vertex

} // end Perspective_To_Screen_OBJECT4DV1

/////////////////////////////////////////////////////////////

void Convert_From_Homogeneous4D_OBJECT4DV1(OBJECT4DV1_PTR obj)
{
	// this function convertes all vertices in the transformed
	// vertex list from 4D homogeneous coordinates to normal 3D coordinates
	// by dividing each x,y,z component by w

	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		// convert to non-homogenous coords
		obj->vlist_trans[vertex].VECTOR4D_DIV_BY_W(&obj->vlist_trans[vertex]);
	} // end for vertex

} // end Convert_From_Homogeneous4D_OBJECT4DV1

/////////////////////////////////////////////////////////////

void CPLGLoader::Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, // render list to transform
	MATRIX4X4_PTR mt,   // transformation matrix
	int coord_select)   // selects coords to transform
{
	// this function simply transforms all of the polygons vertices in the local or trans
	// array of the render list by the sent matrix

	// what coordinates should be transformed?
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
								 for (int poly = 0; poly < rend_list->num_polys; poly++)
								 {
									 // acquire current polygon
									 POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

									 // is this polygon valid?
									 // transform this polygon if and only if it's not clipped, not culled,
									 // active, and visible, note however the concept of "backface" is 
									 // irrelevant in a wire frame engine though
									 if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
										 (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
										 (curr_poly->state & POLY4DV1_STATE_BACKFACE))
										 continue; // move onto next poly

									 // all good, let's transform 
									 for (int vertex = 0; vertex < 3; vertex++)
									 {
										 // transform the vertex by mt
										 POINT4D presult; // hold result of each transformation

										 // transform point
// 										 mt->Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &presult);
// 
// 										 // store result back
// 										 presult.VECTOR4D_COPY(&curr_poly->vlist[vertex], &presult);
										 curr_poly->vlist[vertex].Multiply(*mt);
									 } // end for vertex

								 } // end for poly

	} break;

	case TRANSFORM_TRANS_ONLY:
	{
								 // transform each "transformed" vertex of the render list
								 // remember, the idea of the tvlist[] array is to accumulate
								 // transformations
								 for (int poly = 0; poly < rend_list->num_polys; poly++)
								 {
									 // acquire current polygon
									 POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

									 // is this polygon valid?
									 // transform this polygon if and only if it's not clipped, not culled,
									 // active, and visible, note however the concept of "backface" is 
									 // irrelevant in a wire frame engine though
									 if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
										 (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
										 (curr_poly->state & POLY4DV1_STATE_BACKFACE))
										 continue; // move onto next poly

									 // all good, let's transform 
									 for (int vertex = 0; vertex < 3; vertex++)
									 {
										 // transform the vertex by mt
										 POINT4D presult; // hold result of each transformation

										 // transform point
// 										 mt->Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], mt, &presult);
// 
// 										 // store result back
// 										 presult.VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
										 curr_poly->tvlist[vertex].Multiply(*mt);
									 } // end for vertex

								 } // end for poly

	} break;

	case TRANSFORM_LOCAL_TO_TRANS:
	{
									 // transform each local/model vertex of the render list and store result
									 // in "transformed" vertex list
									 for (int poly = 0; poly < rend_list->num_polys; poly++)
									 {
										 // acquire current polygon
										 POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

										 // is this polygon valid?
										 // transform this polygon if and only if it's not clipped, not culled,
										 // active, and visible, note however the concept of "backface" is 
										 // irrelevant in a wire frame engine though
										 if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
											 (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
											 (curr_poly->state & POLY4DV1_STATE_BACKFACE))
											 continue; // move onto next poly

										 // all good, let's transform 
										 for (int vertex = 0; vertex < 3; vertex++)
										 {
											 // transform the vertex by mt
											 curr_poly->tvlist[vertex] = mt->Multiply(curr_poly->vlist[vertex]);
										 } // end for vertex

									 } // end for poly

	} break;

	default: break;

	} // end switch

} // end Transform_RENDERLIST4DV1

/////////////////////////////////////////////////////////////////////////

void CPLGLoader::Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	int coord_select)
{
	// NOTE: Not matrix based 不基于矩阵
	// this function converts the local model coordinates of the
	// sent render list into world coordinates, the results are stored
	// in the transformed vertex list (tvlist) within the renderlist
	// 这个函数转换这个本地模型坐标到世界坐标在渲染列表中，转换结果存储在转换顶点列表（tvlist）在渲染列表中。

	// interate thru vertex list and transform all the model/local 
	// coords to world coords by translating the vertex list by
	// the amount world_pos and storing the results in tvlist[]
	// is this polygon valid?

	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			// acquire current polygon
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

			// transform this polygon if and only if it's not clipped, not culled,
			// active, and visible, note however the concept of "backface" is 
			// irrelevant in a wire frame engine though
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
				(curr_poly->state & POLY4DV1_STATE_BACKFACE))
				continue; // move onto next poly

			// all good, let's transform 
			for (int vertex = 0; vertex < 3; vertex++)
			{
				// translate vertex
				curr_poly->tvlist[vertex] = CVector4D::Add(curr_poly->vlist[vertex], curr_poly->worldPos);
			} // end for vertex

		} // end for poly
	} // end if local
	else // TRANSFORM_TRANS_ONLY
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			// acquire current polygon
			POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

			// transform this polygon if and only if it's not clipped, not culled,
			// active, and visible, note however the concept of "backface" is 
			// irrelevant in a wire frame engine though
			if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
				(curr_poly->state & POLY4DV1_STATE_BACKFACE))
				continue; // move onto next poly

			for (int vertex = 0; vertex < 3; vertex++)
			{
				// translate vertex
				curr_poly->tvlist[vertex] = CVector4D::Add(curr_poly->tvlist[vertex], curr_poly->worldPos);
			} // end for vertex

		} // end for poly

	} // end else

} // end Model_To_World_RENDERLIST4DV1

////////////////////////////////////////////////////////////

void Convert_From_Homogeneous4D_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list)
{
	// this function convertes all valid polygons vertices in the transformed
	// vertex list from 4D homogeneous coordinates to normal 3D coordinates
	// by dividing each x,y,z component by w

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// transform this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concept of "backface" is 
		// irrelevant in a wire frame engine though
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// all good, let's transform 
		for (int vertex = 0; vertex < 3; vertex++)
		{
			// convert to non-homogenous coords
			curr_poly->tvlist[vertex].VECTOR4D_DIV_BY_W(&curr_poly->tvlist[vertex]);
		} // end for vertex

	} // end for poly

} // end Convert_From_Homogeneous4D_RENDERLIST4DV1

/////////////////////////////////////////////////////////////////////////

void CPLGLoader::World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	CAM4DV1_PTR cam)
{
	World_To_Camera_RENDERLIST4DV1(rend_list, &cam->mcam);

} // end World_To_Camera_RENDERLIST4DV1

///////////////////////////////////////////////////////////////

void CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// 这不是一个基于矩阵的函数
	// this function transforms each polygon in the global render list
	// 这个函数转换每一个多边形在全局渲染列表中
	// into perspective coordinates, based on the sent camera object, 
	// 到透视图坐标，基于设置的相机对象
	// you would use this function instead of the object based function
	// 你应该使用这个函数代替这个对象的基函数
	// if you decided earlier in the pipeline to turn each object into 
	// 如果你最初决定
	// a list of polygons and then add them to the global render list

	// transform each polygon in the render list into camera coordinates
	// assumes the render list has already been transformed to world
	// coordinates and the result is in tvlist[] of each polygon object

	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// transform this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concept of "backface" is 
		// irrelevant in a wire frame engine though
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// all good, let's transform 
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;

			// transform the vertex by the view parameters in the camera
			//curr_poly->tvlist[vertex].x = cam->view_dist*curr_poly->tvlist[vertex].x / z;
			//curr_poly->tvlist[vertex].y = cam->view_dist*curr_poly->tvlist[vertex].y*cam->aspect_ratio / z;
			curr_poly->tvlist[vertex].x = curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = -curr_poly->tvlist[vertex].y / z;
			// z = z, so no change

			// not that we are NOT dividing by the homogenous w coordinate since
			// we are not using a matrix operation for this version of the function 

		} // end for vertex

	} // end for poly

} // end Camera_To_Perspective_RENDERLIST4DV1

////////////////////////////////////////////////////////////////

void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the camera coordinates of an object
	// into Screen scaled perspective coordinates, based on the 
	// sent camera object, that is, view_dist_h and view_dist_v 
	// should be set to cause the desired (viewport_width X viewport_height)
	// it only works on the vertices in the tvlist[] list
	// finally, the function also inverts the y axis, so the coordinates
	// generated from this function ARE screen coordinates and ready for
	// rendering

	// transform each polygon in the render list to perspective screen 
	// coordinates assumes the render list has already been transformed 
	// to camera coordinates and the result is in tvlist[]
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// transform this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concept of "backface" is 
		// irrelevant in a wire frame engine though
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		float alpha = (0.5*cam->viewport_width - 0.5);
		float beta = (0.5*cam->viewport_height - 0.5);

		// all good, let's transform 
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;

			// transform the vertex by the view parameters in the camera
			curr_poly->tvlist[vertex].x = cam->view_dist*curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist*curr_poly->tvlist[vertex].y / z;
			// z = z, so no change

			// not that we are NOT dividing by the homogenous w coordinate since
			// we are not using a matrix operation for this version of the function 

			// now the coordinates are in the range x:(-viewport_width/2 to viewport_width/2)
			// and y:(-viewport_height/2 to viewport_height/2), thus we need a translation and
			// since the y-axis is inverted, we need to invert y to complete the screen 
			// transform:
			curr_poly->tvlist[vertex].x = curr_poly->tvlist[vertex].x + alpha;
			curr_poly->tvlist[vertex].y = -curr_poly->tvlist[vertex].y + beta;

		} // end for vertex

	} // end for poly

} // end Camera_To_Perspective_Screen_RENDERLIST4DV1

//////////////////////////////////////////////////////////////

void CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	CAM4DV1_PTR cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the perspective coordinates of the render
	// list into screen coordinates, based on the sent viewport in the camera
	// assuming that the viewplane coordinates were normalized
	// you would use this function instead of the object based function
	// if you decided earlier in the pipeline to turn each object into 
	// a list of polygons and then add them to the global render list
	// you would only call this function if you previously performed
	// a normalized perspective transform

	// transform each polygon in the render list from perspective to screen 
	// coordinates assumes the render list has already been transformed 
	// to normalized perspective coordinates and the result is in tvlist[]
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

		// is this polygon valid?
		// transform this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concept of "backface" is 
		// irrelevant in a wire frame engine though
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		float alpha = (0.5*cam->viewport_width - 0.5);
		float beta = (0.5*cam->viewport_height - 0.5);

		// all good, let's transform 
		for (int vertex = 0; vertex < 3; vertex++)
		{

			// the vertex is in perspective normalized coords from -1 to 1
			// on each axis, simple scale them and invert y axis and project
			// to screen

			// transform the vertex by the view parameters in the camera
			curr_poly->tvlist[vertex].x = alpha + alpha*curr_poly->tvlist[vertex].x;
			curr_poly->tvlist[vertex].y = beta - beta *curr_poly->tvlist[vertex].y;
		} // end for vertex

	} // end for poly

} // end Perspective_To_Screen_RENDERLIST4DV1

///////////////////////////////////////////////////////////////

void CPLGLoader::Reset_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list)
{
	// this function intializes and resets the sent render list and
	// redies it for polygons/faces to be inserted into it
	// note that the render list in this version is composed
	// of an array FACE4DV1 pointer objects, you call this
	// function each frame

	// since we are tracking the number of polys in the
	// list via num_polys we can set it to 0
	// but later we will want a more robust scheme if
	// we generalize the linked list more and disconnect
	// it from the polygon pointer list
	rend_list->num_polys = 0; // that was hard!

}  // Reset_RENDERLIST4DV1

////////////////////////////////////////////////////////////////

void CPLGLoader::Reset_OBJECT4DV1(OBJECT4DV1_PTR obj)
{
	// this function resets the sent object and redies it for 
	// transformations, basically just resets the culled, clipped and
	// backface flags, but here's where you would add stuff
	// to ready any object for the pipeline
	// the object is valid, let's rip it apart polygon by polygon

	// reset object's culled flag
	RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);

	// now the clipped and backface flags for the polygons 
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1_PTR curr_poly = &obj->plist[poly];

		// first is this polygon even visible?
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE))
			continue; // move onto next poly

		// reset clipped and backface flags
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);

	} // end for poly

} // end Reset_OBJECT4DV1

///////////////////////////////////////////////////////////////

int Insert_POLY4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	POLY4DV1_PTR poly)
{
	// converts the sent POLY4DV1 into a FACE4DV1 and inserts it
	// into the render list

	// step 0: are we full?
	if (rend_list->num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return(0);

	// step 1: copy polygon into next opening in polygon render list

	// point pointer to polygon structure
	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];

	// copy fields
	rend_list->poly_data[rend_list->num_polys].state = poly->state;
	rend_list->poly_data[rend_list->num_polys].attr = poly->attr;
	rend_list->poly_data[rend_list->num_polys].color = poly->color;

	// now copy vertices, be careful! later put a loop, but for now
	// know there are 3 vertices always!
	rend_list->poly_data[rend_list->num_polys].tvlist[0].Copy(poly->vlist[poly->vert[0]]);

	rend_list->poly_data[rend_list->num_polys].tvlist[1].Copy(poly->vlist[poly->vert[1]]);

	rend_list->poly_data[rend_list->num_polys].tvlist[2].Copy(poly->vlist[poly->vert[2]]);

	// and copy into local vertices too
	rend_list->poly_data[rend_list->num_polys].vlist[0].Copy(poly->vlist[poly->vert[0]]);

	rend_list->poly_data[rend_list->num_polys].vlist[1].Copy(poly->vlist[poly->vert[1]]);

	rend_list->poly_data[rend_list->num_polys].vlist[2].Copy(poly->vlist[poly->vert[2]]);

	// now the polygon is loaded into the next free array position, but
	// we need to fix up the links

	// test if this is the first entry
	if (rend_list->num_polys == 0)
	{
		// set pointers to null, could loop them around though to self
		rend_list->poly_data[0].next = NULL;
		rend_list->poly_data[0].prev = NULL;
	} // end if
	else
	{
		// first set this node to point to previous node and next node (null)
		rend_list->poly_data[rend_list->num_polys].next = NULL;
		rend_list->poly_data[rend_list->num_polys].prev =
			&rend_list->poly_data[rend_list->num_polys - 1];

		// now set previous node to point to this node
		rend_list->poly_data[rend_list->num_polys - 1].next =
			&rend_list->poly_data[rend_list->num_polys];
	} // end else

	// increment number of polys in list
	rend_list->num_polys++;

	// return successful insertion
	return(1);

} // end Insert_POLY4DV1_RENDERLIST4DV1

//////////////////////////////////////////////////////////////

int CPLGLoader::Insert_POLYF4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	POLYF4DV1_PTR poly)
{
	// inserts the sent polyface POLYF4DV1 into the render list

	// step 0: are we full?
	if (rend_list->num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return(0);

	// step 1: copy polygon into next opening in polygon render list

	// point pointer to polygon structure
	rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];

	// copy face right into array, thats it
	memcpy((void *)&rend_list->poly_data[rend_list->num_polys], (void *)poly, sizeof(POLYF4DV1));

	// now the polygon is loaded into the next free array position, but
	// we need to fix up the links
	// test if this is the first entry
	if (rend_list->num_polys == 0)
	{
		// set pointers to null, could loop them around though to self
		rend_list->poly_data[0].next = NULL;
		rend_list->poly_data[0].prev = NULL;
	} // end if
	else
	{
		// first set this node to point to previous node and next node (null)
		rend_list->poly_data[rend_list->num_polys].next = NULL;
		rend_list->poly_data[rend_list->num_polys].prev =
			&rend_list->poly_data[rend_list->num_polys - 1];

		// now set previous node to point to this node
		rend_list->poly_data[rend_list->num_polys - 1].next =
			&rend_list->poly_data[rend_list->num_polys];
	} // end else

	// increment number of polys in list
	rend_list->num_polys++;

	// return successful insertion
	return(1);

} // end Insert_POLYF4DV1_RENDERLIST4DV1

//////////////////////////////////////////////////////////////

int CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
	OBJECT4DV1_PTR obj,
	int insert_local)
{
	// converts the entire object into a face list and then inserts
	// the visible, active, non-clipped, non-culled polygons into
	// the render list, also note the flag insert_local control 
	// whether or not the vlist_local or vlist_trans vertex list
	// is used, thus you can insert an object "raw" totally untranformed
	// if you set insert_local to 1, default is 0, that is you would
	// only insert an object after at least the local to world transform

	// is this objective inactive or culled or invisible?
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) ||
		(obj->state & OBJECT4DV1_STATE_CULLED) ||
		!(obj->state & OBJECT4DV1_STATE_VISIBLE))
		return(0);

	// the object is valid, let's rip it apart polygon by polygon
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1_PTR curr_poly = &obj->plist[poly];

		// first is this polygon even visible?
		if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// override vertex list polygon refers to
		// the case that you want the local coords used
		// first save old pointer
		POINT4D_PTR vlist_old = curr_poly->vlist;

		if (insert_local)
			curr_poly->vlist = obj->vlist_local;
		else
			curr_poly->vlist = obj->vlist_trans;

		// now insert this polygon
		if (!Insert_POLY4DV1_RENDERLIST4DV1(rend_list, curr_poly))
		{
			// fix vertex list pointer
			curr_poly->vlist = vlist_old;

			// the whole object didn't fit!
			return(0);
		} // end if

		// fix vertex list pointer
		curr_poly->vlist = vlist_old;

	} // end for

	// return success
	return(1);

} // end Insert_OBJECT4DV1_RENDERLIST4DV1

//////////////////////////////////////////////////////////////

void Draw_OBJECT4DV1_Wire(OBJECT4DV1_PTR obj,
	UCHAR *video_buffer, int lpitch, RECT window)

{
	// this function renders an object to the screen in wireframe, 
	// 8 bit mode, it has no regard at all about hidden surface removal, 
	// etc. the function only exists as an easy way to render an object 
	// without converting it into polygons, the function assumes all 
	// coordinates are screen coordinates, but will perform 2D clipping

	// iterate thru the poly list of the object and simply draw
	// each polygon
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// render this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concecpt of "backface" is 
		// irrelevant in a wire frame engine though
		if (!(obj->plist[poly].state & POLY4DV1_STATE_ACTIVE) ||
			(obj->plist[poly].state & POLY4DV1_STATE_CLIPPED) ||
			(obj->plist[poly].state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];

		// draw the lines now
		
		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y,
			obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window );

		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y,
			obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y,
			obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window);

		// track rendering stats
#ifdef DEBUG_ON
		debug_polys_rendered_per_frame++;
#endif

	} // end for poly

} // end Draw_OBJECT4DV1_Wire

///////////////////////////////////////////////////////////////

void Draw_RENDERLIST4DV1_Wire(RENDERLIST4DV1_PTR rend_list,
	UCHAR *video_buffer, int lpitch, RECT window)
{
	// this function "executes" the render list or in other words
	// draws all the faces in the list in wire frame 8bit mode
	// note there is no need to sort wire frame polygons, but 
	// later we will need to, so hidden surfaces stay hidden
	// also, we leave it to the function to determine the bitdepth
	// and call the correct rasterizer

	// at this point, all we have is a list of polygons and it's time
	// to draw them
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// render this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concecpt of "backface" is 
		// irrelevant in a wire frame engine though
		if (!(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_ACTIVE) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_CLIPPED) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// draw the triangle edge one, note that clipping was already set up
		// by 2D initialization, so line clipper will clip all polys out
		// of the 2D screen/window boundary
		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[0].x,
			rend_list->poly_ptrs[poly]->tvlist[0].y,
			rend_list->poly_ptrs[poly]->tvlist[1].x,
			rend_list->poly_ptrs[poly]->tvlist[1].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[1].x,
			rend_list->poly_ptrs[poly]->tvlist[1].y,
			rend_list->poly_ptrs[poly]->tvlist[2].x,
			rend_list->poly_ptrs[poly]->tvlist[2].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[2].x,
			rend_list->poly_ptrs[poly]->tvlist[2].y,
			rend_list->poly_ptrs[poly]->tvlist[0].x,
			rend_list->poly_ptrs[poly]->tvlist[0].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);
		// track rendering stats
#ifdef DEBUG_ON
		debug_polys_rendered_per_frame++;
#endif

	} // end for poly

} // end Draw_RENDERLIST4DV1_Wire

/////////////////////////////////////////////////////////////

void Draw_OBJECT4DV1_Wire16(OBJECT4DV1_PTR obj,
	UCHAR *video_buffer, int lpitch, RECT window)

{
	// this function renders an object to the screen in wireframe, 
	// 16 bit mode, it has no regard at all about hidden surface removal, 
	// etc. the function only exists as an easy way to render an object 
	// without converting it into polygons, the function assumes all 
	// coordinates are screen coordinates, but will perform 2D clipping

	// iterate thru the poly list of the object and simply draw
	// each polygon
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		// render this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concecpt of "backface" is 
		// irrelevant in a wire frame engine though
		if (!(obj->plist[poly].state & POLY4DV1_STATE_ACTIVE) ||
			(obj->plist[poly].state & POLY4DV1_STATE_CLIPPED) ||
			(obj->plist[poly].state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int vindex_0 = obj->plist[poly].vert[0];
		int vindex_1 = obj->plist[poly].vert[1];
		int vindex_2 = obj->plist[poly].vert[2];

		// draw the lines now
		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y,
			obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_1].x, obj->vlist_trans[vindex_1].y,
			obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(obj->vlist_trans[vindex_2].x, obj->vlist_trans[vindex_2].y,
			obj->vlist_trans[vindex_0].x, obj->vlist_trans[vindex_0].y,
			obj->plist[poly].color,
			video_buffer, lpitch, window);

		// track rendering stats
#ifdef DEBUG_ON
		debug_polys_rendered_per_frame++;
#endif


	} // end for poly

} // end Draw_OBJECT4DV1_Wire16

///////////////////////////////////////////////////////////////

void CPLGLoader::Draw_RENDERLIST4DV1_Wire16(RENDERLIST4DV1_PTR rend_list,
	UCHAR *video_buffer, int lpitch, RECT window)
{
	// this function "executes" the render list or in other words
	// draws all the faces in the list in wire frame 16bit mode
	// note there is no need to sort wire frame polygons, but 
	// later we will need to, so hidden surfaces stay hidden
	// also, we leave it to the function to determine the bitdepth
	// and call the correct rasterizer

	// at this point, all we have is a list of polygons and it's time
	// to draw them
	for (int poly = 0; poly < rend_list->num_polys; poly++)
	{
		// render this polygon if and only if it's not clipped, not culled,
		// active, and visible, note however the concecpt of "backface" is 
		// irrelevant in a wire frame engine though
		if (!(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_ACTIVE) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_CLIPPED) ||
			(rend_list->poly_ptrs[poly]->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		// draw the triangle edge one, note that clipping was already set up
		// by 2D initialization, so line clipper will clip all polys out
		// of the 2D screen/window boundary
		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[0].x,
			rend_list->poly_ptrs[poly]->tvlist[0].y,
			rend_list->poly_ptrs[poly]->tvlist[1].x,
			rend_list->poly_ptrs[poly]->tvlist[1].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[1].x,
			rend_list->poly_ptrs[poly]->tvlist[1].y,
			rend_list->poly_ptrs[poly]->tvlist[2].x,
			rend_list->poly_ptrs[poly]->tvlist[2].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);

		g_pDDraw->DrawClipLine(rend_list->poly_ptrs[poly]->tvlist[2].x,
			rend_list->poly_ptrs[poly]->tvlist[2].y,
			rend_list->poly_ptrs[poly]->tvlist[0].x,
			rend_list->poly_ptrs[poly]->tvlist[0].y,
			rend_list->poly_ptrs[poly]->color,
			video_buffer, lpitch, window);

		// track rendering stats
#ifdef DEBUG_ON
		debug_polys_rendered_per_frame++;
#endif

	} // end for poly

} // end Draw_RENDERLIST4DV1_Wire

/////////////////////////////////////////////////////////////

void CPLGLoader::Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq)
{
	// this creates a camera matrix based on Euler angles 
	// and stores it in the sent camera object
	// if you recall from chapter 6 to create the camera matrix
	// we need to create a transformation matrix that looks like:

	// Mcam = mt(-1) * my(-1) * mx(-1) * mz(-1)
	// that is the inverse of the camera translation matrix mutilplied
	// by the inverses of yxz, in that order, however, the order of
	// the rotation matrices is really up to you, so we aren't going
	// to force any order, thus its programmable based on the value
	// of cam_rot_seq which can be any value CAM_ROT_SEQ_XYZ where 
	// XYZ can be in any order, YXZ, ZXY, etc.

	MATRIX4X4 mt_inv,  // inverse camera translation matrix
		mx_inv,  // inverse camera x axis rotation matrix
		my_inv,  // inverse camera y axis rotation matrix
		mz_inv,  // inverse camera z axis rotation matrix
		mrot,    // concatenated inverse rotation matrices
		mtmp;    // temporary working matrix


	// step 1: create the inverse translation matrix for the camera
	// position
	mt_inv.Initialize( 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);

	// step 2: create the inverse rotation sequence for the camera
	// rember either the transpose of the normal rotation matrix or
	// plugging negative values into each of the rotations will result
	// in an inverse matrix

	// first compute all 3 rotation matrices

	// extract out euler angles
	float theta_x = cam->dir.x;
	float theta_y = cam->dir.y;
	float theta_z = cam->dir.z;

	// compute the sine and cosine of the angle x
	float cos_theta = CMath2::Fast_Cos(theta_x);  // no change since cos(-x) = cos(x)
	float sin_theta = -CMath2::Fast_Sin(theta_x); // sin(-x) = -sin(x)

	// set the matrix up 
	mx_inv.Initialize( 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);

	// compute the sine and cosine of the angle y
	cos_theta = CMath2::Fast_Cos(theta_y);  // no change since cos(-x) = cos(x)
	sin_theta = -CMath2::Fast_Sin(theta_y); // sin(-x) = -sin(x)

	// set the matrix up 
	my_inv.Initialize( cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);

	// compute the sine and cosine of the angle z
	cos_theta = CMath2::Fast_Cos(theta_z);  // no change since cos(-x) = cos(x)
	sin_theta = -CMath2::Fast_Sin(theta_z); // sin(-x) = -sin(x)

	// set the matrix up 
	mz_inv.Initialize( cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	// now compute inverse camera rotation sequence
	switch (cam_rot_seq)
	{
	case CAM_ROT_SEQ_XYZ:
	{
							mtmp.Multiply(&mx_inv, &my_inv);
							mrot.Multiply(&mtmp, &mz_inv);
	} break;

	case CAM_ROT_SEQ_YXZ:
	{
							mtmp.Multiply(&my_inv, &mx_inv);
							mrot.Multiply(&mtmp, &mz_inv);
	} break;

	case CAM_ROT_SEQ_XZY:
	{
							mtmp.Multiply(&mx_inv, &mz_inv);
							mrot.Multiply(&mtmp, &my_inv);
	} break;

	case CAM_ROT_SEQ_YZX:
	{
							mtmp.Multiply(&my_inv, &mz_inv);
							mrot.Multiply(&mtmp, &mx_inv);
	} break;

	case CAM_ROT_SEQ_ZYX:
	{
							mtmp.Multiply(&mz_inv, &my_inv);
							mrot.Multiply(&mtmp, &mx_inv);
	} break;

	case CAM_ROT_SEQ_ZXY:
	{
							mtmp.Multiply(&mz_inv, &mx_inv);
							mrot.Multiply(&mtmp, &my_inv);

	} break;

	default: break;
	} // end switch

	// now mrot holds the concatenated product of inverse rotation matrices
	// multiply the inverse translation matrix against it and store in the 
	// camera objects' camera transform matrix we are done!
	cam->mcam.Multiply(&mt_inv, &mrot);

} // end Build_CAM4DV1_Matrix_Euler

/////////////////////////////////////////////////////////////

void Build_CAM4DV1_Matrix_UVN(CAM4DV1_PTR cam, int mode)
{
	// this creates a camera matrix based on a look at vector n,
	// look up vector v, and a look right (or left) u
	// and stores it in the sent camera object, all values are
	// extracted out of the camera object itself
	// mode selects how uvn is computed
	// UVN_MODE_SIMPLE - low level simple model, use the target and view reference point
	// UVN_MODE_SPHERICAL - spherical mode, the x,y components will be used as the
	//     elevation and heading of the view vector respectively
	//     along with the view reference point as the position
	//     as usual

	MATRIX4X4 mt_inv,  // inverse camera translation matrix
		mt_uvn,  // the final uvn matrix
		mtmp;    // temporary working matrix

	// step 1: create the inverse translation matrix for the camera
	// position
	mt_inv.Initialize( 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);


	// step 2: determine how the target point will be computed
	if (mode == UVN_MODE_SPHERICAL)
	{
		// use spherical construction
		// target needs to be recomputed

		// extract elevation and heading 
		float phi = cam->dir.x; // elevation
		float theta = cam->dir.y; // heading

		// compute trig functions once
		float sin_phi = CMath2::Fast_Sin(phi);
		float cos_phi = CMath2::Fast_Cos(phi);

		float sin_theta = CMath2::Fast_Sin(theta);
		float cos_theta = CMath2::Fast_Cos(theta);

		// now compute the target point on a unit sphere x,y,z
		cam->target.x = -1 * sin_phi*sin_theta;
		cam->target.y = 1 * cos_phi;
		cam->target.z = 1 * sin_phi*cos_theta;
	} // end else

	// at this point, we have the view reference point, the target and that's
	// all we need to recompute u,v,n
	// Step 1: n = <target position - view reference point>
	cam->pos.VECTOR4D_Build(&cam->pos, &cam->target, &cam->n);

	// Step 2: Let v = <0,1,0>
	cam->v.Initialize( 0, 1, 0);

	// Step 3: u = (v x n)
	cam->u = CVector4D::Cross(cam->v,cam->n);

	// Step 4: v = (n x u)
	cam->v = CVector4D::Cross(cam->n, cam->u);

	// Step 5: normalize all vectors
	cam->v.VECTOR4D_Normalize(&cam->u);
	cam->v.VECTOR4D_Normalize(&cam->v);
	cam->v.VECTOR4D_Normalize(&cam->n);


	// build the UVN matrix by placing u,v,n as the columns of the matrix
	mt_uvn.Initialize( cam->u.x, cam->v.x, cam->n.x, 0,
		cam->u.y, cam->v.y, cam->n.y, 0,
		cam->u.z, cam->v.z, cam->n.z, 0,
		0, 0, 0, 1);

	// now multiply the translation matrix and the uvn matrix and store in the 
	// final camera matrix mcam
	cam->mcam.Multiply(&mt_inv, &mt_uvn);

} // end Build_CAM4DV1_Matrix_UVN

/////////////////////////////////////////////////////////////

void CPLGLoader::Init_CAM4DV1(CAM4DV1_PTR cam,       // the camera object
	int cam_attr,          // attributes
	POINT4D_PTR cam_pos,   // initial camera position
	VECTOR4D_PTR cam_dir,  // initial camera angles
	POINT4D_PTR cam_target, // UVN target
	float near_clip_z,     // near and far clipping planes
	float far_clip_z,
	float fov,             // field of view in degrees
	float viewport_width,  // size of final screen viewport
	float viewport_height)
{
	// this function initializes the camera object cam, the function
	// doesn't do a lot of error checking or sanity checking since 
	// I want to allow you to create projections as you wish, also 
	// I tried to minimize the number of parameters the functions needs

	// first set up parms that are no brainers
	cam->attr = cam_attr;              // camera attributes

	cam->pos.Copy(*cam_pos); // positions
	cam->dir.Copy(*cam_dir); // direction vector or angles for
	// euler camera
	// for UVN camera
	cam->u.Initialize( 1, 0, 0);  // set to +x
	cam->v.Initialize( 0, 1, 0);  // set to +y
	cam->n.Initialize( 0, 0, 1);  // set to +z        

	if (cam_target != NULL)
		cam->target.Copy( *cam_target); // UVN target
	else
		cam->target.VECTOR4D_ZERO(&cam->target);

	cam->near_clip_z = near_clip_z;     // near z=constant clipping plane
	cam->far_clip_z = far_clip_z;      // far z=constant clipping plane

	cam->viewport_width = viewport_width;   // dimensions of viewport
	cam->viewport_height = viewport_height;

	cam->viewport_center_x = (viewport_width - 1) / 2; // center of viewport
	cam->viewport_center_y = (viewport_height - 1) / 2;

	cam->aspect_ratio = (float)viewport_width / (float)viewport_height;

	// set all camera matrices to identity matrix
	cam->mcam.Zero();
	cam->mper.Zero();
	cam->mscr.Zero();

	// set independent vars
	cam->fov = fov;

	// set the viewplane dimensions up, they will be 2 x (2/ar)
	cam->viewplane_width = 2.0;
	cam->viewplane_height = 2.0 / cam->aspect_ratio;

	// now we know fov and we know the viewplane dimensions plug into formula and
	// solve for view distance parameters
	float tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));

	cam->view_dist = (0.5)*(cam->viewplane_width)*tan_fov_div2;

	// test for 90 fov first since it's easy :)
	if (fov == 90.0)
	{
		// set up the clipping planes -- easy for 90 degrees!
		POINT3D pt_origin; // point on the plane
		pt_origin.Initialize( 0, 0, 0);

		VECTOR3D vn; // normal to plane

		// right clipping plane 
		vn.Initialize( 1, 0, -1); // x=z plane
		cam->rt_clip_plane.Initialize(pt_origin, vn, 1);

		// left clipping plane
		vn.Initialize(-1, 0, -1); // -x=z plane
		cam->lt_clip_plane.Initialize(pt_origin, vn, 1);

		// top clipping plane
		vn.Initialize( 0, 1, -1); // y=z plane
		cam->tp_clip_plane.Initialize(pt_origin, vn, 1);

		// bottom clipping plane
		vn.Initialize( 0, -1, -1); // -y=z plane
		cam->bt_clip_plane.Initialize(pt_origin, vn, 1);
	} // end if d=1
	else
	{
		// now compute clipping planes yuck!
		POINT3D pt_origin; // point on the plane
		pt_origin.Initialize( 0, 0, 0);

		VECTOR3D vn; // normal to plane

		// since we don't have a 90 fov, computing the normals
		// are a bit tricky, there are a number of geometric constructions
		// that solve the problem, but I'm going to solve for the
		// vectors that represent the 2D projections of the frustrum planes
		// on the x-z and y-z planes and then find perpendiculars to them

		// right clipping plane, check the math on graph paper 
		vn.Initialize( cam->view_dist, 0, -cam->viewplane_width / 2.0);
		cam->rt_clip_plane.Initialize( pt_origin, vn, 1);

		// left clipping plane, we can simply reflect the right normal about
		// the z axis since the planes are symetric about the z axis
		// thus invert x only
		vn.Initialize(-cam->view_dist, 0, -cam->viewplane_width / 2.0);
		cam->lt_clip_plane.Initialize(pt_origin, vn, 1);

		// top clipping plane, same construction
		vn.Initialize( 0, cam->view_dist, -cam->viewplane_width / 2.0);
		cam->tp_clip_plane.Initialize( pt_origin, vn, 1);

		// bottom clipping plane, same inversion
		vn.Initialize( 0, -cam->view_dist, -cam->viewplane_width / 2.0);
		cam->bt_clip_plane.Initialize( pt_origin, vn, 1);
	} // end else

} // end Init_CAM4DV1