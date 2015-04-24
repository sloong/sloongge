#pragma once

namespace SoaringLoong
{
	namespace Math
	{
		namespace Matrix
		{
			class CMatrix4x4;
		}
		namespace Vector
		{
			class CVector4D;
		}
		namespace Polygon
		{
			class CPolygon3D;
			class IPolygon;
		}
	}
	using namespace Math::Vector;
	using namespace Math::Matrix;
	using namespace Math::Polygon;
	namespace Graphics
	{
		class CDDraw;
	}
	using Graphics::CDDraw;
	namespace Graphics3D
	{
		class CCamera;
		class SLOONGENGINE_API CObject3D
		{
		public:
			CObject3D();
			~CObject3D();

		public:
			virtual void Update();
			virtual void Render( CDDraw* pDDraw );
			virtual void DeleteBackface(const CCamera& pCam);
			virtual void UpdateWorldVertex(const CVector4D& mWorld);
			virtual void UpdateCameraVertex(const CMatrix4x4& mCamera);
			virtual void UpdateProjectVertex(const CMatrix4x4& mProject);
			virtual void UpdateScreenVertex(const CMatrix4x4& mScreen);
			virtual void UpdateVertex(const CMatrix4x4& mMarix, bool bNormal);
			virtual void AddPolygon(IPolygon* pPoly);
		public:
			CMatrix4x4*			m_pCameraMatrix;
			CMatrix4x4*			m_pScreenMatrix;
			CMatrix4x4*			m_pProjectMatrix;
			CVector4D*			m_pWorldPos;
			CCamera*			m_pCamera;
			vector<IPolygon*>*	m_VertexList;
		};
	}
}