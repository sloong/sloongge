#include "stdafx.h"
#include "SloongPolygon.h"
#include "SloongVector.h"
#include "SloongDraw.h"
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Math::Polygon;


// these are the matrix versions, note they are more inefficient for
// single transforms, but their power comes into play when you concatenate
// multiple transformations, not to mention that all transforms are accomplished
// with the same code, just the matrix differs

int CPolygon2D::Translate_Polygon2D_Mat(CPolygon2D* poly, int dx, int dy)
{
	// this function translates the center of a polygon by using a matrix multiply
	// on the the center point, this is incredibly inefficient, but for educational purposes
	// if we had an object that wasn't in local coordinates then it would make more sense to
	// use a matrix, but since the origin of the object is at x0,y0 then 2 lines of code can
	// translate, but lets do it the hard way just to see :)

	// test for valid pointer
	if (!poly)
		return(0);

	CMatrix3x2 mt; // used to hold translation transform matrix

	// initialize the matrix with translation values dx dy
	mt.Mat_Init_3X2(&mt, 1, 0, 0, 1, dx, dy);

	// create a 1x2 matrix to do the transform
	CMatrix1x2 p0 = { poly->x0, poly->y0 };
	CMatrix1x2 p1 = { 0, 0 }; // this will hold result

	// now translate via a matrix multiply
	mt.Mat_Mul_1X2_3X2(&p0, &mt, &p1);

	// now copy the result back into polygon
	poly->x0 = p1.M[0];
	poly->y0 = p1.M[1];
	/*
	double lfTemp[] = { 1, 0, 0, 1, dx, dy };
	CMatrix oMatrix3x2(3, 2, lfTemp);

	// create a 1x2 matrix to do the transform
	double lf1x2[] = { poly->x0, poly->y0, 0 };
	CMatrix oMatix1x2(1, 3, lf1x2);
	// now translate via a matrix multiply
	CMatrix oRes = oMatix1x2*oMatrix3x2;
	// now copy the result back into polygon
	poly->x0 = (int)oRes.m_pMatrix[0][0];
	poly->y0 = (int)oRes.m_pMatrix[0][1];*/


	// return success
	return(1);

} // end Translate_Polygon2D_Mat

///////////////////////////////////////////////////////////////

int CPolygon2D::Rotate_Polygon2D_Mat(CPolygon2D* poly, int theta)
{
	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// test for negative rotation angle
	if (theta < 0)
		theta += 360;

	CMatrix3x2 mr; // used to hold rotation transform matrix

	// initialize the matrix with translation values dx dy
	mr.Mat_Init_3X2(&mr, CMathBase::cos_look[theta], CMathBase::sin_look[theta],
		-CMathBase::sin_look[theta], CMathBase::cos_look[theta],
		0, 0);

	// loop and rotate each point, very crude, no lookup!!!
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// create a 1x2 matrix to do the transform
		CMatrix1x2 p0 = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y };
		CMatrix1x2 p1 = { 0, 0 }; // this will hold result

		// now rotate via a matrix multiply
		mr.Mat_Mul_1X2_3X2(&p0, &mr, &p1);

		// now copy the result back into vertex
		poly->vlist[curr_vert].x = p1.M[0];
		poly->vlist[curr_vert].y = p1.M[1];

	} // end for curr_vert

	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	// 	if (!poly)
	// 		return(0);
	// 
	// 	// test for negative rotation angle
	// 	if (theta < 0)
	// 		theta += 360;
	// 
	// 	try
	// 	{
	// 		// used to hold rotation transform matrix
	// 		/*	// initialize the matrix with translation values dx dy
	// 		double lfTemp[] = { cos_look[theta], sin_look[theta],
	// 		-sin_look[theta], cos_look[theta],
	// 		0, 0 };
	// 		CMatrix oMatrix3x2(3, 2, lfTemp);
	// 
	// 		// loop and rotate each point, very crude, no lookup!!!
	// 		for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	// 		{
	// 		// create a 1x2 matrix to do the transform
	// 		double lf1x2[] = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y, 0 };
	// 		CMatrix oMatix1x2(1, 3, lf1x2);
	// 
	// 		// now rotate via a matrix multiply
	// 		//Mat_Mul1X2_3X2(&p0, &mr, &p1);
	// 		CMatrix oRes = oMatix1x2*oMatrix3x2;
	// 
	// 		// now copy the result back into vertex
	// 		poly->vlist[curr_vert].x = oRes.m_pMatrix[0][0];
	// 		poly->vlist[curr_vert].y = oRes.m_pMatrix[0][1];
	// 
	// 		} // end for curr_vert*/
	// 	}
	// 	catch (LPCTSTR strErr)
	// 	{
	// 		//g_oLog.ResLog(ERR,1,strErr);
	// 	}
	// return success
	return(1);

} // end Rotate_Polygon2D_Mat

////////////////////////////////////////////////////////

int CPolygon2D::Scale_Polygon2D_Mat(CPolygon2D* poly, float sx, float sy)
{
	// this function scalesthe local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);


	CMatrix3x2 ms; // used to hold scaling transform matrix

	// initialize the matrix with translation values dx dy
	ms.Mat_Init_3X2(&ms, sx, 0,
		0, sy,
		0, 0);


	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// scale and store result back

		// create a 1x2 matrix to do the transform
		CMatrix1x2 p0 = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y };
		CMatrix1x2 p1 = { 0, 0 }; // this will hold result

		// now scale via a matrix multiply
		ms.Mat_Mul_1X2_3X2(&p0, &ms, &p1);

		// now copy the result back into vertex
		poly->vlist[curr_vert].x = p1.M[0];
		poly->vlist[curr_vert].y = p1.M[1];

	} // end for curr_vert
	/*	// used to hold scaling transform matrix
	// initialize the matrix with translation values dx dy
	double lfTemp[] = { sx, 0,
	0, sy,
	0, 0 };
	CMatrix oMatrix3x2(3, 2, lfTemp);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
	// scale and store result back

	// create a 1x2 matrix to do the transform
	double lf1x2[] = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y, 0 };
	CMatrix oMatix1x2(1, 3, lf1x2);
	// now scale via a matrix multiply

	// now copy the result back into vertex
	CMatrix oRes = oMatix1x2*oMatrix3x2;

	// now copy the result back into vertex
	poly->vlist[curr_vert].x = oRes.m_pMatrix[0][0];
	poly->vlist[curr_vert].y = oRes.m_pMatrix[0][1];

	} // end for curr_vert*/
	// return success
	return(1);

} // end Scale_Polygon2D_Mat

int CPolygon2D::Scale_Polygon2D(CPolygon2D* poly, float sx, float sy)
{
	// this function scalesthe local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// scale and store result back
		poly->vlist[curr_vert].x *= sx;
		poly->vlist[curr_vert].y *= sy;

	} // end for curr_vert

	// return success
	return(1);

} // end Scale_Polygon2D

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

// the following 3 functions are the standard transforms (no matrices)

int CPolygon2D::Translate_Polygon2D(CPolygon2D* poly, int dx, int dy)
{
	// this function translates the center of a polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// translate
	poly->x0 += dx;
	poly->y0 += dy;

	// return success
	return(1);

} // end Translate_Polygon2D


int CPolygon2D::Rotate_Polygon2D(CPolygon2D* poly, int theta)
{
	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// test for negative rotation angle
	if (theta < 0)
		theta += 360;

	// loop and rotate each point, very crude, no lookup!!!
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{

		// perform rotation
		double xr = (double)poly->vlist[curr_vert].x*CMathBase::cos_look[theta] -
			(double)poly->vlist[curr_vert].y*CMathBase::sin_look[theta];

		double yr = (double)poly->vlist[curr_vert].x*CMathBase::sin_look[theta] +
			(double)poly->vlist[curr_vert].y*CMathBase::cos_look[theta];

		// store result back
		poly->vlist[curr_vert].x = xr;
		poly->vlist[curr_vert].y = yr;

	} // end for curr_vert

	// return success
	return(1);

} // end Rotate_Polygon2D

HRESULT CPolygon3D::Render()
{
	auto& list = *m_pTransList;
	auto x = list[m_n1];
	auto y = list[m_n2];
	auto z = list[m_n3];

	m_pDDraw->DrawClipLine(x->x, x->y, y->x, y->y, m_dwColor);
	m_pDDraw->DrawClipLine(y->x, y->y, z->x, z->y, m_dwColor);
	m_pDDraw->DrawClipLine(x->x, x->y, z->x, z->y, m_dwColor);

	return S_OK;
}

SoaringLoong::Math::Polygon::CPolygon3D::CPolygon3D(CDDraw* pDraw)
{
	m_pTransList = new vector<CVector4D*>;
	m_pLocalList = new vector<CVector4D*>;
	m_pDDraw = pDraw;
}

CPolygon3D::~CPolygon3D()
{
 	SAFE_DELETE(m_pLocalList);
 	SAFE_DELETE(m_pTransList);
}

void CPolygon3D::Initialize(int n1, int n2, int n3, vector<CVector4D*>* pLocalList, vector<CVector4D*>* pTransList)
{
	m_n1 = (n1);
	m_n2 = (n2);
	m_n3 = (n3);
	m_pLocalList = pLocalList;
	m_pTransList = pTransList;
}

void CPolygon3D::SetAttribute(DWORD arrt)
{
	m_dwAttribute = arrt;
}

void CPolygon3D::AddAttribute(DWORD arrt)
{
	m_dwAttribute |= arrt;
}

void CPolygon3D::ToWorld(const CVector4D& vWorld)
{
	// 对多边形的每一个点进行转换
// 	for (int i = 0; i < ARRAYSIZE(m_VectorList); i++)
// 	{
// 		m_RenderList[i] = (CVector4D::Add((m_VectorList)[i], vWorld));
// 	}


// 	void Build_Model_To_World_MATRIX4X4(VECTOR4D_PTR vpos, MATRIX4X4_PTR m)
// 	{
// 		// this function builds up a general local to world 
// 		// transformation matrix that is really nothing more than a translation
// 		// of the origin by the amount specified in vpos
// 
// 		m->Initialize(1, 0, 0, 0,
// 			0, 1, 0, 0,
// 			0, 0, 1, 0,
// 			vpos->x, vpos->y, vpos->z, 1);
// 
// 	} // end Build_Model_To_World_MATRIX4X4

}

void CPolygon3D::Transform(const CMatrix4x4& mMatrix, bool bNormal)
{
// 	for (int i = 0; i < ARRAYSIZE(m_VectorList); i++)
// 	{
// 		auto temp = (CVector4D::Multiply((m_VectorList)[i], mMatrix));
// 		if (bNormal && temp.w != 1 && temp.x > 0 &&  temp.y >0 && temp.z > 0)
// 		{
// 			temp.x /= temp.w;
// 			temp.y /= temp.w;
// 			temp.z /= temp.w;
// 			temp.w = 1;
// 		}
// 
// 		(m_RenderList)[i] = temp;
// 	}
}

void CPolygon3D::SetColor(COLORREF color)
{
	m_dwColor = color;
}

COLORREF CPolygon3D::GetColor()
{
	return m_dwColor;
}

void CPolygon3D::SetStatus(DWORD dwStatus)
{
	m_dwStatus = dwStatus;
}

DWORD CPolygon3D::GetStatus()
{
	return m_dwStatus;
}

DWORD CPolygon3D::GetAttribute()
{
	return m_dwAttribute;
}

void CPolygon3D::GetIndex(int& x, int& y, int& z)
{
	x = m_n1;
	y = m_n2;
	z = m_n3;
}

void CPolygon3D::AddStatus(DWORD dwStatus)
{
	m_dwStatus |= dwStatus;
}

void CPolygon3D::DeleteStatus(DWORD dwStatus)
{
	m_dwStatus &= (~dwStatus);
}

SLOONGMATH_API IPolygon* SoaringLoong::Math::Polygon::IPolygon::Create3D(CDDraw* pDraw)
{
	IPolygon* pNew = new CPolygon3D(pDraw);
	return pNew;
}
