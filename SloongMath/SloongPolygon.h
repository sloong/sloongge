#pragma once

#include "ISloongPolygon.h"


// states of polygons and faces
#define POLY4DV1_STATE_ACTIVE             0x0001
#define POLY4DV1_STATE_CLIPPED            0x0002
#define POLY4DV1_STATE_BACKFACE           0x0004


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


namespace SoaringLoong
{
	namespace Math
	{
		namespace Polygon
		{
			class CPolygon2D
			{
			public:
				int Translate_Polygon2D_Mat(CPolygon2D* poly, int dx, int dy);
				int Rotate_Polygon2D_Mat(CPolygon2D* poly, int theta);
				int Scale_Polygon2D_Mat(CPolygon2D* poly, float sx, float sy);

				int Translate_Polygon2D(CPolygon2D* poly, int dx, int dy);
				int Rotate_Polygon2D(CPolygon2D* poly, int theta);
				int Scale_Polygon2D(CPolygon2D* poly, float sx, float sy);

			public:
				int state;        // state of polygon
				int num_verts;    // number of vertices
				int x0, y0;        // position of center of polygon  
				int xv, yv;        // initial velocity
				DWORD color;      // could be index or PALETTENTRY
				VERTEX2DF *vlist; // pointer to vertex list
			};

			class CPolygon3D : public IPolygon
			{
			public:
				CPolygon3D(CDDraw* pDraw);
				virtual ~CPolygon3D();
			public:
				void	Initialize(int n1, int n2, int n3, vector<CVector4D*>* pLocalList, vector<CVector4D*>* pTransList);
				HRESULT Render();
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

				
			protected:
				DWORD				m_dwAttribute;
				DWORD				m_dwStatus;
				COLORREF			m_dwColor;
				int					m_n1, m_n2, m_n3;
				vector<CVector4D*>*	m_pLocalList;
				vector<CVector4D*>*	m_pTransList;
				CDDraw*				m_pDDraw;
			};

		}
	}
}
