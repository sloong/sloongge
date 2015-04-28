#include "stdafx.h"
#include "SloongObject3D.h"
#include "SloongVector.h"
#include "SloongMatrix.h"
#include "SloongPolygon.h"
#include "SloongCamera.h"
#include "SloongDraw.h"
#include "SloongException.h"
#include "SloongFile.h"
using namespace SoaringLoong::Graphics3D;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Math::Matrix;
using namespace SoaringLoong::Math::Polygon;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Universal;
SoaringLoong::Graphics3D::CObject3D::CObject3D(CDDraw* pDDraw)
{
	m_pWorldPos = nullptr;
	m_pCamera = nullptr;
	m_pCameraMatrix = nullptr;
	m_pProjectMatrix = nullptr;
	m_pScreenMatrix = nullptr;
	m_pDDraw = pDDraw;

	m_pPolygonList = new vector<IPolygon*>;
	m_pLocalList = new vector<CVector4D*>;
	m_pTransList = new vector<CVector4D*>;
}

SoaringLoong::Graphics3D::CObject3D::~CObject3D()
{
	SAFE_DELETE(m_pWorldPos);
	m_pCamera = nullptr;
	m_pCameraMatrix = nullptr;
	m_pProjectMatrix = nullptr;
	m_pScreenMatrix = nullptr;
	for (int i = 0; i < m_nNumPolygones; i++)
	{
		auto cur = (*m_pPolygonList)[i];
		SAFE_DELETE(cur);
	}
	for (int i = 0; i < m_nNumVertices; i++)
	{
		auto cur = (*m_pTransList)[i];
		SAFE_DELETE(cur);
		cur = (*m_pLocalList)[i];
		SAFE_DELETE(cur);
	}

	SAFE_DELETE(m_pPolygonList);
	SAFE_DELETE(m_pLocalList);
	SAFE_DELETE(m_pTransList);
}

void SoaringLoong::Graphics3D::CObject3D::Update()
{
	UpdateWorldVertex(*m_pWorldPos);
	//DeleteBackface()
	UpdateCameraVertex(*m_pCameraMatrix);
	UpdateProjectVertex(*m_pProjectMatrix);
	UpdateScreenVertex(*m_pScreenMatrix);
}

void SoaringLoong::Graphics3D::CObject3D::Render(CDDraw* pDDraw)
{
	if ( !Visible())
	{
		return;
	}

	LPBYTE pBuffer = pDDraw->DDraw_Lock_Back_Surface();

	auto list = *m_pPolygonList;
	auto len = list.size();
	for (int i = 0; i < len; i++)
	{
		if (list[i])
		{
			if (!(list[i]->GetStatus() & POLY4DV1_STATE_ACTIVE) ||
				(list[i]->GetStatus() & POLY4DV1_STATE_CLIPPED) ||
				(list[i]->GetStatus() & POLY4DV1_STATE_BACKFACE))
				continue;
			list[i]->Render(pDDraw);
		}

	}
	pDDraw->DDraw_Unlock_Back_Surface();
}

void SoaringLoong::Graphics3D::CObject3D::UpdateWorldVertex(const POINT4D& mWorld)
{
	if (!m_pWorldPos)
	{
		m_pWorldPos = new CVector4D();
	}
	m_pWorldPos->Copy(mWorld);
	ToWorld(TRANS_MODE::LocalToTrans);
}


void SoaringLoong::Graphics3D::CObject3D::UpdateCameraVertex(const CMatrix4x4& mCamera)
{
	if (!m_pCameraMatrix)
	{
		m_pCameraMatrix = new CMatrix4x4();
	}
	m_pCameraMatrix->Copy(mCamera);
	UpdateVertex(mCamera, false);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateProjectVertex(const CMatrix4x4& mProject)
{
	if (!m_pProjectMatrix)
	{
		m_pProjectMatrix = new CMatrix4x4();
	}
	m_pProjectMatrix->Copy(mProject);
	UpdateVertex(mProject, true);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateScreenVertex(const CMatrix4x4& mScreen)
{
	if (!m_pScreenMatrix)
	{
		m_pScreenMatrix = new CMatrix4x4();
	}
	m_pScreenMatrix->Copy(mScreen);
	UpdateVertex(mScreen, true);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateVertex(const CMatrix4x4& mMarix, bool bNormal)
{
	auto len = m_pPolygonList->size();
	for (int i = 0; i < len; i++)
	{
		if ((*m_pPolygonList)[i])
		{
			(*m_pPolygonList)[i]->Transform(mMarix, bNormal);
		}

	}
}

void SoaringLoong::Graphics3D::CObject3D::AddPolygon(IPolygon* pPoly)
{
	m_pPolygonList->push_back(pPoly);
}

void SoaringLoong::Graphics3D::CObject3D::AddVertex(const CVector4D& vVertex)
{
	CVector4D* pLocal = new CVector4D();
	CVector4D* pTrans = new CVector4D();
	pLocal->Copy(vVertex);
	pTrans->Copy(vVertex);
	m_pLocalList->push_back(pLocal);
	m_pTransList->push_back(pTrans);
}

void SoaringLoong::Graphics3D::CObject3D::ComputeRadius()
{
	// this function computes the average and maximum radius for 
	// sent object and opdates the object data

	// reset incase there's any residue
	m_fAvgRadius = 0;
	m_fMaxRadius = 0;

	// loop thru and compute radius
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		auto vVertex = (*m_pLocalList)[vertex];
		// update the average and maximum radius
		float dist_to_vertex =
			sqrt(vVertex->x*vVertex->x +
			vVertex->y*vVertex->y +
			vVertex->z*vVertex->z);

		// accumulate total radius
		m_fAvgRadius += dist_to_vertex;

		// update maximum radius   
		if (dist_to_vertex > m_fMaxRadius)
			m_fMaxRadius = dist_to_vertex;

	} // end for vertex

	// finallize average radius computation
	m_fAvgRadius /= m_nNumVertices;
}

CVector4D* SoaringLoong::Graphics3D::CObject3D::GetVertex(int nIndex)
{
	if (nIndex < m_nNumVertices)
	{
		return (*m_pLocalList)[nIndex];
	}
	else
		return nullptr;
}

void SoaringLoong::Graphics3D::CObject3D::Reset()
{
	// this function resets the sent object and redies it for 
	// transformations, basically just resets the culled, clipped and
	// backface flags, but here's where you would add stuff
	// to ready any object for the pipeline
	// the object is valid, let's rip it apart polygon by polygon

	// reset object's culled flag
	DeleteStatus(OBJECT4DV1_STATE_CULLED);

	// now the clipped and backface flags for the polygons 
	for (int poly = 0; poly < m_nNumPolygones; poly++)
	{
		// acquire polygon
		auto curPoly = (*m_pPolygonList)[poly];
		//POLY4DV1_PTR curr_poly = &obj->plist[poly];

		// first is this polygon even visible?
		if (!(curPoly->GetStatus() & POLY4DV1_STATE_ACTIVE))
			continue; // move onto next poly

		// reset clipped and backface flags
		curPoly->DeleteStatus(POLY4DV1_STATE_CLIPPED);
		curPoly->DeleteStatus(POLY4DV1_STATE_BACKFACE);

	} // end for poly
}

void SoaringLoong::Graphics3D::CObject3D::Transform(const CMatrix4x4& mMatrix, TRANS_MODE emMode, bool transform_basis)
{
	// this function simply transforms all of the vertices in the local or trans
	// array by the sent matrix

	// what coordinates should be transformed?
	switch (emMode)
	{
	case LocalOnly:
	{
					  // transform each local/model vertex of the object mesh in place
					  for (int vertex = 0; vertex < m_nNumVertices; vertex++)
					  {
						  POINT4D presult; // hold result of each transformation

						  // transform point
						  auto vVertex = (*m_pLocalList)[vertex];
						  vVertex->Multiply(mMatrix);
					  } // end for index
	} break;

	case TransOnly:
	{
					  // transform each "transformed" vertex of the object mesh in place
					  // remember, the idea of the vlist_trans[] array is to accumulate
					  // transformations
					  for (int vertex = 0; vertex < m_nNumVertices; vertex++)
					  {
						  POINT4D presult; // hold result of each transformation

						  // transform point
						  (*m_pTransList)[vertex]->Multiply(mMatrix);
					  } // end for index			
	} break;

	case LocalToTrans:
	{
						 // transform each local/model vertex of the object mesh and store result
						 // in "transformed" vertex list
						 for (int vertex = 0; vertex < m_nNumVertices; vertex++)
						 {
							 POINT4D presult; // hold result of each transformation

							 // transform point
							 (*m_pTransList)[vertex]->Copy(mMatrix.Multiply((*(*m_pLocalList)[vertex])));
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
		m_vUx.Multiply(mMatrix);

		// rotate uy of basis
		//mt->Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		//vresult.VECTOR4D_COPY(&obj->uy, &vresult);
		m_vUy.Multiply(mMatrix);

		// rotate uz of basis
		//mt->Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
		//vresult.VECTOR4D_COPY(&obj->uz, &vresult);
		m_vUz.Multiply(mMatrix);
	} // end if

}

void SoaringLoong::Graphics3D::CObject3D::LoadPLGMode(LPCTSTR strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRot)
{
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
	//memset(obj, 0, sizeof(OBJECT4DV1));

	// set state of object to active and visible
	this->SetStatus(OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE);


	// set position of object
	this->SetWorldPosition(vPos);

	// Step 2: open the file for reading
	CFile oFile;
	oFile.OpenStream(strFileName, _T("r"));

	// Step 3: get the first token string which should be the object descriptor
	LPCTSTR token_string;  // pointer to actual token text, ready for parsing
	token_string = oFile.GetLine();
	if (token_string == NULL)
	{
		throw CException(CString(_T("PLG file error with file %s (object descriptor invalid)."), strFileName));
	} // end if

	//Write_Error("Object Descriptor: %s", token_string);

	// parse out the info object
	TCHAR szTemp[65] = { 0 };
	_stscanf_s(token_string, _T("%s %d %d"), szTemp, 64, &this->m_nNumVertices, &this->m_nNumPolygones);
	m_strName = szTemp;

	// Step 4: load the vertex list
	for (int vertex = 0; vertex < this->m_nNumVertices; vertex++)
	{
		// get the next vertex
		if (!(token_string = oFile.GetLine()))
		{
			throw CException(CString(_T("PLG file error with file %s (vertex list invalid)."), strFileName));
			return;
		} // end if


		CVector4D vTemp;
		vTemp.Zero();

		// parse out vertex
		stscanf_s(token_string, _T("%f %f %f"), &vTemp.x, &vTemp.y, &vTemp.z);
		vTemp.w = 1;

		// scale vertices
		vTemp.x *= vScale.x;
		vTemp.y *= vScale.y;
		vTemp.z *= vScale.z;

		this->AddVertex(vTemp);

	} // end for vertex

	// compute average and max radius
	this->ComputeRadius();

	// 	Write_Error("\nObject average radius = %f, max radius = %f",
	// 		obj->avg_radius, obj->max_radius);

	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts = 0; // number of vertices for current poly (always 3)
	TCHAR tmp_string[8];        // temp string to hold surface descriptor in and
	// test if it need to be converted from hex

	// Step 5: load the polygon list
	for (int poly = 0; poly < this->m_nNumPolygones; poly++)
	{
		// get the next polygon descriptor
		if (!(token_string = oFile.GetLine()))
		{
			throw CException(CString(_T("PLG file error with file %s (polygon descriptor invalid)."), strFileName));
		} // end if

		//Write_Error("\nPolygon %d:", poly);

		IPolygon* pPoly = IPolygon::Create3D();

		int n1, n2, n3;
		// each vertex list MUST have 3 vertices since we made this a rule that all models
		// must be constructed of triangles
		// read in surface descriptor, number of vertices, and vertex list
		stscanf_s(token_string, _T("%s %d %d %d %d"), tmp_string, 8,
			&poly_num_verts, // should always be 3 
			&n1, &n2, &n3);

		pPoly->Initialize(n1, n2, n3, m_pLocalList, m_pTransList);
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
		/*obj->plist[poly].vlist = obj->vlist_local;*/

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
			pPoly->AddAttribute(POLY4DV1_ATTR_2SIDED);
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
			pPoly->AddAttribute(POLY4DV1_ATTR_RGB16);

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
			pPoly->SetColor(RGB(red * 16, green * 16, blue * 16));
			//Write_Error("\nRGB color = [%d, %d, %d]", red, green, blue);
		} // end if
		else
		{
			// this is an 8-bit color indexed surface
			pPoly->AddAttribute(POLY4DV1_ATTR_8BITCOLOR);

			// and simple extract the last 8 bits and that's the color index
			pPoly->SetColor(poly_surface_desc & 0x00ff);

			//Write_Error("\n8-bit color index = %d", obj->plist[poly].color);

		} // end else

		// handle shading mode
		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);
		DWORD attr;
		// set polygon shading mode
		switch (shade_mode)
		{
		case PLX_SHADE_MODE_PURE_FLAG: {
										   attr = POLY4DV1_ATTR_SHADE_MODE_PURE;
										   // Write_Error("\nShade mode = pure");
		} break;

		case PLX_SHADE_MODE_FLAT_FLAG: {
										   attr = POLY4DV1_ATTR_SHADE_MODE_FLAT;
										   //Write_Error("\nShade mode = flat");

		} break;

		case PLX_SHADE_MODE_GOURAUD_FLAG: {
											  attr = POLY4DV1_ATTR_SHADE_MODE_GOURAUD;
											  //Write_Error("\nShade mode = gouraud");
		} break;

		case PLX_SHADE_MODE_PHONG_FLAG: {
											attr = POLY4DV1_ATTR_SHADE_MODE_PHONG;
											//Write_Error("\nShade mode = phong");
		} break;

		default: break;
		} // end switch

		pPoly->AddAttribute(attr);
		// finally set the polygon to active
		pPoly->SetStatus(POLY4DV1_STATE_ACTIVE);

		AddPolygon(pPoly);
	} // end for poly
}

void SoaringLoong::Graphics3D::CObject3D::Move(const CVector4D& vTrans)
{
	// NOTE: Not matrix based
	// this function translates an object without matrices,
	// simply updates the world_pos
	m_pWorldPos->Add(vTrans);
}

void SoaringLoong::Graphics3D::CObject3D::Scale(const CVector4D& vScale)
{
	// NOTE: Not matrix based
	// this function scales and object without matrices 
	// modifies the object's local vertex list 
	// additionally the radii is updated for the object

	// for each vertex in the mesh scale the local coordinates by
	// vs on a componentwise basis, that is, sx, sy, sz
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		auto vCur = (*m_pLocalList)[vertex];
		vCur->x *= vScale.x;
		vCur->y *= vScale.y;
		vCur->z *= vScale.z;
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
	float scale = MAX(vScale.x, vScale.y);
	scale = MAX(scale, vScale.z);

	// now scale
	m_fMaxRadius *= scale;
	m_fAvgRadius *= scale;

}

void SoaringLoong::Graphics3D::CObject3D::Rotate(float x, float y, float z)
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
	mrot.BuildRotateMatrix(x, y, z);

	// now simply rotate each point of the mesh in local/model coordinates
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		POINT4D presult; // hold result of each transformation

		// transform point
		// 		mrot.Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], &mrot, &presult);
		// 
		// 		// store result back
		// 		presult.VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);
		(*m_pLocalList)[vertex]->Multiply(mrot);

	} // end for index

	// now rotate orientation basis for object
	VECTOR4D vresult; // use to rotate each orientation vector axis

	// rotate ux of basis
	// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->ux, &mrot, &vresult);
	// 	vresult.VECTOR4D_COPY(&obj->ux, &vresult);
	m_vUx.Multiply(mrot);

	// rotate uy of basis
	// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->uy, &mrot, &vresult);
	// 	vresult.VECTOR4D_COPY(&obj->uy, &vresult);
	m_vUy.Multiply(mrot);

	// rotate uz of basis
	// 	mrot.Mat_Mul_VECTOR4D_4X4(&obj->uz, &mrot, &vresult);
	// 	vresult.VECTOR4D_COPY(&obj->uz, &vresult);
	m_vUz.Multiply(mrot);

}

void SoaringLoong::Graphics3D::CObject3D::ToWorld(TRANS_MODE emMode)
{
	// NOTE: Not matrix based
	// this function converts the local model coordinates of the
	// sent object into world coordinates, the results are stored
	// in the transformed vertex list (vlist_trans) within the object

	// interate thru vertex list and transform all the model/local 
	// coords to world coords by translating the vertex list by
	// the amount world_pos and storing the results in vlist_trans[]

	if (emMode == TRANS_MODE::LocalToTrans)
	{
		for (int vertex = 0; vertex < m_nNumVertices; vertex++)
		{
			// translate vertex
			(*m_pTransList)[vertex]->Copy(CVector4D::Add((*m_pLocalList)[vertex], m_pWorldPos));
		} // end for vertex
	} // end if local
	else
	{ // TRANSFORM_TRANS_ONLY
		for (int vertex = 0; vertex < m_nNumVertices; vertex++)
		{
			// translate vertex
			(*m_pTransList)[vertex]->Add(m_pWorldPos);
		} // end for vertex
	} // end else trans

}

void SoaringLoong::Graphics3D::CObject3D::Cull(CCamera* cam, CULL_MODE emMode)
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
	sphere_pos = cam->MatrixCamera.Multiply(m_pWorldPos);

	// step 2:  based on culling flags remove the object
	if (emMode & CULL_ON_Z_PLANE)
	{
		// cull only based on z clipping planes

		// test far plane
		if (((sphere_pos.z - m_fMaxRadius) > cam->FarZ) ||
			((sphere_pos.z + m_fMaxRadius) < cam->NearZ))
		{
			AddStatus(OBJECT4DV1_STATE_CULLED);
			return;
		} // end if

	} // end if

	if (emMode & CULL_ON_X_PLANE)
	{
		// cull only based on x clipping planes
		// we could use plane equations, but simple similar triangles
		// is easier since this is really a 2D problem
		// if the view volume is 90 degrees the the problem is trivial
		// buts lets assume its not

		// test the the right and left clipping planes against the leftmost and rightmost
		// points of the bounding sphere
		float z_test = (0.5)*cam->ViewPlaneWidth*sphere_pos.z / cam->ViewDistance;

		if (((sphere_pos.x - m_fMaxRadius) > z_test) || // right side
			((sphere_pos.x + m_fMaxRadius) < -z_test))  // left side, note sign change
		{
			AddStatus(OBJECT4DV1_STATE_CULLED);
			return;
		} // end if
	} // end if

	if (emMode & CULL_ON_Y_PLANE)
	{
		// cull only based on y clipping planes
		// we could use plane equations, but simple similar triangles
		// is easier since this is really a 2D problem
		// if the view volume is 90 degrees the the problem is trivial
		// buts lets assume its not

		// test the the top and bottom clipping planes against the bottommost and topmost
		// points of the bounding sphere
		float z_test = (0.5)*cam->ViewPlaneHeight*sphere_pos.z / cam->ViewDistance;

		if (((sphere_pos.y - m_fMaxRadius) > z_test) || // top side
			((sphere_pos.y + m_fMaxRadius) < -z_test))  // bottom side, note sign change
		{
			AddStatus(OBJECT4DV1_STATE_CULLED);
			return;
		} // end if

	} // end if

}

void SoaringLoong::Graphics3D::CObject3D::AddStatus(DWORD dwStatus)
{
	m_dwStatus |= dwStatus;
}

void SoaringLoong::Graphics3D::CObject3D::SetStatus(DWORD dwStatus)
{
	m_dwStatus = dwStatus;
}

DWORD SoaringLoong::Graphics3D::CObject3D::GetStatus()
{
	return m_dwStatus;
}

DWORD SoaringLoong::Graphics3D::CObject3D::GetAttribute()
{
	return m_dwAttribute;
}

void SoaringLoong::Graphics3D::CObject3D::SetAttribute(DWORD dwAttribute)
{
	m_dwAttribute = dwAttribute;
}

void SoaringLoong::Graphics3D::CObject3D::AddAttribute(DWORD dwAttribute)
{
	m_dwAttribute |= dwAttribute;
}

void SoaringLoong::Graphics3D::CObject3D::RemoveBackface(CCamera* pCam)
{
	// NOTE: this is not a matrix based function
	// this function removes the backfaces from an object's
	// polygon mesh, the function does this based on the vertex
	// data in vlist_trans along with the camera position (only)
	// note that only the backface state is set in each polygon

	// test if the object is culled
	if (GetStatus() & OBJECT4DV1_STATE_CULLED)
		return;

	// process each poly in mesh
	for (int poly = 0; poly < m_nNumPolygones; poly++)
	{
		// acquire polygon
		auto cur = (*m_pPolygonList)[poly];

		// is this polygon valid?
		// test this polygon if and only if it's not clipped, not culled,
		// active, and visible and not 2 sided. Note we test for backface in the event that
		// a previous call might have already determined this, so why work
		// harder!
		if (!(cur->GetStatus() & POLY4DV1_STATE_ACTIVE) ||
			(cur->GetStatus() & POLY4DV1_STATE_CLIPPED) ||
			(cur->GetStatus() & POLY4DV1_STATE_BACKFACE) ||
			(cur->GetAttribute()  & POLY4DV1_ATTR_2SIDED))
			continue; // move onto next poly

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int x, y, z;
		cur->GetIndex(x, y, z);

		// we will use the transformed polygon vertex list since the backface removal
		// only makes sense at the world coord stage further of the pipeline 

		// we need to compute the normal of this polygon face, and recall
		// that the vertices are in cw order, u = p0->p1, v=p0->p2, n=uxv
		VECTOR4D u, v, n;

		// build u, v
		u.Subtract((*m_pTransList)[x], (*m_pTransList)[y]);
		v.Subtract((*m_pTransList)[x], (*m_pTransList)[z]);

		// compute cross product
		n = CVector4D::Cross(u, v);

		// now create eye vector to viewpoint
		VECTOR4D view;
		view.Subtract((*m_pTransList)[x], &pCam->WorldPos);

		// and finally, compute the dot product
		float dp = n.Dot(view);

		// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
		if (dp <= 0.0)
			cur->AddStatus(POLY4DV1_STATE_BACKFACE);

	} // end for poly

}

void SoaringLoong::Graphics3D::CObject3D::ToCamera(CCamera* pCam)
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
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		// transform the vertex by the mcam matrix within the camera
		// it better be valid!
		POINT4D presult; // hold result of each transformation

		// transform point
		(*m_pTransList)[vertex]->Multiply(pCam->MatrixCamera);
	} // end for vertex


}

void SoaringLoong::Graphics3D::CObject3D::ToProject(CCamera* pCam)
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
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		auto cur = (*m_pTransList)[vertex];

		// transform the vertex by the view parameters in the camera
		cur->x = pCam->ViewDistance*cur->x / cur->z;
		cur->y = pCam->ViewDistance*cur->y*pCam->AspectRatio / cur->z;
		// z = z, so no change

		// not that we are NOT dividing by the homogenous w coordinate since
		// we are not using a matrix operation for this version of the function 

	} // end for vertex

}

void SoaringLoong::Graphics3D::CObject3D::CameraToPerspectiveScreen(CCamera* pCam)
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

	float alpha = (0.5*pCam->ScreenWidth - 0.5);
	float beta = (0.5*pCam->ScreenHeight - 0.5);

	// transform each vertex in the object to perspective screen coordinates
	// assumes the object has already been transformed to camera
	// coordinates and the result is in vlist_trans[]
	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		auto cur = (*m_pTransList)[vertex];

		// transform the vertex by the view parameters in the camera
		cur->x = pCam->ViewDistance*cur->x / cur->z;
		cur->y = pCam->ViewDistance*cur->y / cur->z;
		// z = z, so no change

		// not that we are NOT dividing by the homogenous w coordinate since
		// we are not using a matrix operation for this version of the function 

		// now the coordinates are in the range x:(-viewport_width/2 to viewport_width/2)
		// and y:(-viewport_height/2 to viewport_height/2), thus we need a translation and
		// since the y-axis is inverted, we need to invert y to complete the screen 
		// transform:
		cur->x = cur->x + alpha;
		cur->y = -cur->y + beta;

	} // end for vertex

}

void SoaringLoong::Graphics3D::CObject3D::PerspectiveToScreen(CCamera* pCam)
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

	float alpha = (0.5*pCam->ScreenWidth - 0.5);
	float beta = (0.5*pCam->ScreenHeight - 0.5);

	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		// assumes the vertex is in perspective normalized coords from -1 to 1
		// on each axis, simple scale them to viewport and invert y axis and project
		// to screen
		auto cur = (*m_pTransList)[vertex];

		// transform the vertex by the view parameters in the camera
		cur->x = alpha + alpha*cur->x;
		cur->y = beta - beta *cur->y;

	} // end for vertex

}

void SoaringLoong::Graphics3D::CObject3D::ConvertFromHomogeneous4D()
{
	// this function convertes all vertices in the transformed
	// vertex list from 4D homogeneous coordinates to normal 3D coordinates
	// by dividing each x,y,z component by w

	for (int vertex = 0; vertex < m_nNumVertices; vertex++)
	{
		// convert to non-homogenous coords
		auto cur = (*m_pTransList)[vertex];
		cur->VECTOR4D_DIV_BY_W(cur);
	} // end for vertex

}

void SoaringLoong::Graphics3D::CObject3D::SetWorldPosition(const CVector4D& vPos)
{
	if ( !m_pWorldPos )
	{
		m_pWorldPos = new CVector4D();
	}
	m_pWorldPos->Copy(vPos);
}

SoaringLoong::Math::Vector::CVector4D SoaringLoong::Graphics3D::CObject3D::GetWorldPosition()
{
	return *m_pWorldPos;
}

bool SoaringLoong::Graphics3D::CObject3D::Visible()
{
	if (!(GetStatus() & OBJECT4DV1_STATE_ACTIVE) ||
		(GetStatus() & OBJECT4DV1_STATE_CULLED) ||
		!(GetStatus() & OBJECT4DV1_STATE_VISIBLE))
		return false;
	else
		return true;
}

void SoaringLoong::Graphics3D::CObject3D::DeleteStatus(DWORD dwStatus)
{
	m_dwStatus &= (~dwStatus);
}
