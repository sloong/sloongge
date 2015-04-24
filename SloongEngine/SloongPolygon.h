#pragma once

#include "SloongVertex.h"
#include "SloongMatrix.h"
#include "SloongVector.h"
using namespace SoaringLoong::Math::Vertex;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Math::Matrix;
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
			class SLOONGENGINE_API CPolygon2D
			{
			public:
				int state;        // state of polygon
				int num_verts;    // number of vertices
				int x0, y0;        // position of center of polygon  
				int xv, yv;        // initial velocity
				DWORD color;      // could be index or PALETTENTRY
				VERTEX2DF *vlist; // pointer to vertex list
			};

			class SLOONGENGINE_API IPolygon
			{
			public:
				static  IPolygon* Create3D();
				virtual void Initialize(const CVector4D& v1, const CVector4D& v2, const CVector4D& v3) = 0;
				virtual HRESULT Render(CDDraw* pDraw){ return S_OK; };

				virtual CVector4D* GetX(){ return NULL; };
				virtual CVector4D* GetY(){ return NULL; };
				virtual CVector4D* GetZ(){ return NULL; };

				virtual void SetAttribute(DWORD arrt){}
				virtual void AddAttribute(DWORD arrt){}

				virtual void ToWorld(const CVector4D& vWorld){}
				virtual void Transform(const CMatrix4x4& mMatrix, bool toNormal){}
			};

			
		}
	}
}
