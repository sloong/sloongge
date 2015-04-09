#pragma once

#include "SloongVertex.h"
using namespace SoaringLoong::Math::Vertex;

namespace SoaringLoong
{
	namespace Graphics
	{
		class CDDraw;
	}
	using Graphics::CDDraw;
	namespace Math
	{
		namespace Vector
		{
			class CVector4D;
		}
		using Vector::CVector4D;
		namespace Polygon
		{
			typedef enum PolygonStateType
			{
				POLY_STATE_ACTIVE = 0x0001,
				POLY_STATE_CLIPPED = 0x0002,
				POLY_STATE_BACKFACE = 0x0004,
			}POLYSTATE;
			class CPolygon2D
			{
			public:
				int state;        // state of polygon
				int num_verts;    // number of vertices
				int x0, y0;        // position of center of polygon  
				int xv, yv;        // initial velocity
				DWORD color;      // could be index or PALETTENTRY
				VERTEX2DF *vlist; // pointer to vertex list
			};

			class CPolygon3D
			{
			public:
				HRESULT Render(CDDraw* pDraw);
			public:
				vector<CVector4D>	m_VectorList;
				vector<CVector4D>	m_WorldVertex;
				vector<CVector4D>	m_CameraVertexList;
				vector<CVector4D>	m_ProjectVertexList;
				vector<CVector4D>	m_ScreenVertexList;
				DWORD				m_dwAttribute;
				COLORREF			m_dwColor;
			};
		}
	}
}
