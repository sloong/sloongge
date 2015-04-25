#include "stdafx.h"
#include "SloongPolygon.h"
#include "SloongDraw.h"
#include "SloongVector.h"
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Polygon;


class CPolygon3D : public IPolygon
{
public:
	CPolygon3D();
	virtual ~CPolygon3D();
public:
	void	Initialize(const CVector4D& v1, const CVector4D& v2, const CVector4D& v3);
	HRESULT Render(CDDraw* pDraw);
	CVector4D* GetX();
	CVector4D* GetY();
	CVector4D* GetZ();

	void SetAttribute(DWORD arrt);
	void AddAttribute(DWORD arrt);

	void ToWorld(const CVector4D& vWorld);
	void Transform(const CMatrix4x4& mMatrix, bool toNormal);
public:
	DWORD				m_dwAttribute;
	COLORREF			m_dwColor;
	vector<CVector4D>	m_VectorList;
	vector<CVector4D>	m_RenderList;
};


HRESULT CPolygon3D::Render(CDDraw* pDraw)
{
	int index = 0;
	auto vList = m_RenderList;
	for (index = 0; index < vList.size() - 1; index++)
	{
		// draw line from ith to ith+1 vertex
		pDraw->DrawClipLine((int)vList[index].x,
			(int)vList[index].y,
			(int)vList[index + 1].x,
			(int)vList[index + 1].y,
			RGB(255,255,255));

	} // end for
	return S_OK;
}

CPolygon3D::CPolygon3D()
{

}

CPolygon3D::~CPolygon3D()
{

}

void CPolygon3D::Initialize(const CVector4D& v1, const CVector4D& v2, const CVector4D& v3)
{
	m_VectorList.push_back(v1);
	m_VectorList.push_back(v2);
	m_VectorList.push_back(v3);
}

SoaringLoong::Math::CVector4D* CPolygon3D::GetX()
{
	return &m_VectorList[0];
}

SoaringLoong::Math::CVector4D* CPolygon3D::GetY()
{
	return &m_VectorList[1];
}

SoaringLoong::Math::CVector4D* CPolygon3D::GetZ()
{
	return &m_VectorList[2];
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
	m_RenderList = m_VectorList;
	// 对多边形的每一个点进行转换
	for (int i = 0; i < m_VectorList.size(); i++)
	{
		m_RenderList[i] = (CVector4D::Add((m_VectorList)[i], vWorld));
	}


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
	for (int i = 0; i < m_VectorList.size(); i++)
	{
		auto temp = (CVector4D::Multiply((m_VectorList)[i], mMatrix));
		if (bNormal && temp.w != 1 && temp.x > 0 &&  temp.y >0 && temp.z > 0)
		{
			temp.x /= temp.w;
			temp.y /= temp.w;
			temp.z /= temp.w;
			temp.w = 1;
		}

		(m_RenderList)[i] = temp;
	}
}

SLOONGENGINE_API IPolygon* SoaringLoong::Math::Polygon::IPolygon::Create3D()
{
	IPolygon* pNew = new CPolygon3D();
	return pNew;
}
