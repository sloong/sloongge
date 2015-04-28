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
	void	Initialize(int n1, int n2, int n3, VectorList* pLocalList, VectorList* pTransList);
	HRESULT Render(CDDraw* pDraw);
	void	GetIndex(int& x, int& y, int& z);

	void	SetAttribute(DWORD arrt);
	void	AddAttribute(DWORD arrt);
	DWORD	GetAttribute();

	void	ToWorld(const CVector4D& vWorld);
	void	Transform(const CMatrix4x4& mMatrix, bool toNormal);

	void	SetColor(COLORREF color);
	COLORREF GetColor();

	void SetStatus(DWORD dwStatus);
	void AddStatus(DWORD dwStatus);
	void DeleteStatus(DWORD dwStatus);
	DWORD GetStatus();
public:
	DWORD				m_dwAttribute;
	DWORD				m_dwStatus;
	COLORREF			m_dwColor;
	int					m_n1, m_n2, m_n3;
	VectorList*			m_pLocalList;
	VectorList*			m_pTransList;
};


HRESULT CPolygon3D::Render(CDDraw* pDraw)
{
	auto list = *m_pTransList;
	auto x = list[m_n1];
	auto y = list[m_n2];
	auto z = list[m_n3];

	pDraw->DrawClipLine(x->x, x->y, y->x, y->y, m_dwColor);
	pDraw->DrawClipLine(y->x, y->y, z->x, z->y, m_dwColor);
	pDraw->DrawClipLine(x->x, x->y, z->x, z->y, m_dwColor);

	return S_OK;
}

CPolygon3D::CPolygon3D()
{

}

CPolygon3D::~CPolygon3D()
{

}

void CPolygon3D::Initialize(int n1, int n2, int n3, VectorList* pLocalList, VectorList* pTransList)
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

SLOONGENGINE_API IPolygon* SoaringLoong::Math::Polygon::IPolygon::Create3D()
{
	IPolygon* pNew = new CPolygon3D();
	return pNew;
}
