#pragma once

#include "SloongVertex.h"
#include "SloongMatrix.h"
#include "SloongVector.h"
using namespace SoaringLoong::Math::Vertex;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Math::Matrix;


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
	namespace Graphics
	{
		class CDDraw;
	}
	using Graphics::CDDraw;
	namespace Math
	{
		namespace Polygon
		{
			typedef enum PolygonStateType
			{
				POLY_STATE_ACTIVE = 0x0001,
				POLY_STATE_CLIPPED = 0x0002,
				POLY_STATE_BACKFACE = 0x0004,
			}POLYSTATE;

			class SLOONGENGINE_API IPolygon
			{
			public:
				static  IPolygon* Create3D();
				virtual void Initialize(int n1, int n2, int n3, vector<CVector4D*>* pLocalList, vector<CVector4D*>* pTransList) = 0;
				virtual HRESULT Render(CDDraw* pDraw) = 0;

				virtual void GetIndex(int& x, int& y, int& z) = 0;

				virtual void SetAttribute(DWORD arrt) = 0;
				virtual void AddAttribute(DWORD arrt) = 0;
				virtual DWORD GetAttribute() = 0;

				virtual void ToWorld(const CVector4D& vWorld) = 0;
				virtual void Transform(const CMatrix4x4& mMatrix, bool toNormal) = 0;

				virtual void SetColor(COLORREF color) = 0;
				virtual COLORREF GetColor() = 0;

				virtual void SetStatus(DWORD dwStatus) = 0;
				virtual void AddStatus(DWORD dwStatus) = 0;
				virtual void DeleteStatus(DWORD dwStatus) = 0;
				virtual DWORD GetStatus() = 0;
			};

		}
	}
}
