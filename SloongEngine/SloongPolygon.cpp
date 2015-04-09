#include "stdafx.h"
#include "SloongPolygon.h"
#include "SloongDraw.h"
#include "SloongVector.h"
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Math::Vector;
HRESULT SoaringLoong::Math::Polygon::CPolygon3D::Render(CDDraw* pDraw)
{
	int index = 0;
	for (index = 0; index < m_ScreenVertexList.size()-1; index++)
	{
		// draw line from ith to ith+1 vertex
		pDraw->DrawClipLine((int)m_ScreenVertexList[index].x,
			(int)m_ScreenVertexList[index].y,
			(int)m_ScreenVertexList[index + 1].x,
			(int)m_ScreenVertexList[index + 1].y,
			m_dwColor);

	} // end for
	return S_OK;
}
