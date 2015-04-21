#include "stdafx.h"
#include "SloongPolygon.h"
#include "SloongDraw.h"
#include "SloongVector.h"
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Math::Vector;
HRESULT SoaringLoong::Math::Polygon::CPolygon3D::Render(CDDraw* pDraw)
{
	int index = 0;
	auto vList = *m_ScreenVertexList;
	for (index = 0; index < vList.size() - 1; index++)
	{
		// draw line from ith to ith+1 vertex
		pDraw->DrawClipLine((int)vList[index].x,
			(int)vList[index].y,
			(int)vList[index + 1].x,
			(int)vList[index + 1].y,
			m_dwColor);

	} // end for
	return S_OK;
}

SoaringLoong::Math::Polygon::CPolygon3D::CPolygon3D()
{
	m_VectorList = new vector<CVector4D>;
	m_WorldVertex = new vector<CVector4D>;
	m_CameraVertexList = new vector<CVector4D>;
	m_ProjectVertexList = new vector<CVector4D>;
	m_ScreenVertexList = new vector<CVector4D>;
}

SoaringLoong::Math::Polygon::CPolygon3D::~CPolygon3D()
{
	SAFE_DELETE(m_VectorList);
	SAFE_DELETE(m_WorldVertex);
	SAFE_DELETE(m_CameraVertexList);
	SAFE_DELETE(m_ProjectVertexList);
	SAFE_DELETE(m_ScreenVertexList);
}
