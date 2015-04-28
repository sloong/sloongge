#pragma once

#include "SloongString.h"


// states for objects
#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

// defines for enhanced PLG file format -> PLX
// the surface descriptor is still 16-bit now in the following format
// d15                      d0
//   CSSD | RRRR| GGGG | BBBB

// C is the RGB/indexed color flag
// SS are two bits that define the shading mode
// D is the double sided flag
// and RRRR, GGGG, BBBB are the red, green, blue bits for RGB mode
// or GGGGBBBB is the 8-bit color index for 8-bit mode

// bit masks to simplify testing????
#define PLX_RGB_MASK          0x8000   // mask to extract RGB or indexed color
#define PLX_SHADE_MODE_MASK   0x6000   // mask to extract shading mode
#define PLX_2SIDED_MASK       0x1000   // mask for double sided
#define PLX_COLOR_MASK        0x0fff   // xxxxrrrrggggbbbb, 4-bits per channel RGB
// xxxxxxxxiiiiiiii, indexed mode 8-bit index

// these are the comparision flags after masking

// shading mode of polygon
#define PLX_SHADE_MODE_PURE_FLAG      0x0000  // this poly is a constant color
#define PLX_SHADE_MODE_CONSTANT_FLAG  0x0000  // alias
#define PLX_SHADE_MODE_FLAT_FLAG      0x2000  // this poly uses flat shading
#define PLX_SHADE_MODE_GOURAUD_FLAG   0x4000  // this poly used gouraud shading
#define PLX_SHADE_MODE_PHONG_FLAG     0x6000  // this poly uses phong shading
#define PLX_SHADE_MODE_FASTPHONG_FLAG 0x6000  // this poly uses phong shading (alias)

// double sided flag
#define PLX_2SIDED_FLAG              0x1000   // this poly is double sided
#define PLX_1SIDED_FLAG              0x0000   // this poly is single sided
// color mode of polygon
#define PLX_COLOR_MODE_RGB_FLAG     0x8000   // this poly uses RGB color
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000   // this poly uses an indexed 8-bit color 


#include "SloongVector.h"
namespace SoaringLoong
{
	using Universal::CString;
	namespace Math
	{
		namespace Matrix
		{
			class CMatrix4x4;
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
		typedef enum _TransformMode
		{
			LocalOnly,
			TransOnly,
			LocalToTrans,
		}TRANS_MODE;

		typedef enum _CullMode
		{
			CULL_ON_X_PLANE = 0x0001, // cull on the x clipping planes
			CULL_ON_Y_PLANE = 0x0002, // cull on the y clipping planes
			CULL_ON_Z_PLANE = 0x0004, // cull on the z clipping planes
			CULL_ON_XYZ_PLANES = (CULL_ON_X_PLANE | CULL_ON_Y_PLANE | CULL_ON_Z_PLANE),
		}CULL_MODE;
		class CCamera;
		class SLOONGENGINE_API CObject3D
		{
		public:
			CObject3D( CDDraw* pDDraw );
			~CObject3D();

		public:
			virtual void LoadPLGMode(LPCTSTR strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRot);
			virtual void Update();
			virtual void Render( CDDraw* pDDraw );
			virtual void UpdateWorldVertex(const CVector4D& mWorld);
			virtual void UpdateCameraVertex(const CMatrix4x4& mCamera);
			virtual void UpdateProjectVertex(const CMatrix4x4& mProject);
			virtual void UpdateScreenVertex(const CMatrix4x4& mScreen);
			virtual void UpdateVertex(const CMatrix4x4& mMarix, bool bNormal);
			virtual void AddPolygon(IPolygon* pPoly);
			virtual void AddVertex(const CVector4D& vVertex);
			virtual CVector4D* GetVertex(int nIndex);
			virtual void AddAttribute(DWORD dwAttribute);
			virtual void SetAttribute(DWORD dwAttribute);
			virtual DWORD GetAttribute();
			virtual void AddStatus(DWORD dwStatus);
			virtual void DeleteStatus(DWORD dwStatus);
			virtual void SetStatus(DWORD dwStatus);
			virtual DWORD GetStatus();

			virtual void SetWorldPosition(const CVector4D& vPos);
			virtual CVector4D GetWorldPosition();

			virtual void ComputeRadius();

			virtual void Reset();

			virtual void Transform(const CMatrix4x4& mMatrix, TRANS_MODE emMode, bool transform_basis);
			virtual void Move(const CVector4D& vTrans);
			virtual void Scale(const CVector4D& vScale);

			virtual void Rotate(float x, float y, float z);

			virtual void ToWorld(TRANS_MODE emMode);
			virtual void ToCamera(CCamera* pCam);
			virtual void ToProject(CCamera* pCam);
			virtual void CameraToPerspectiveScreen(CCamera* pCam);
			virtual void PerspectiveToScreen(CCamera* pCam);
			virtual void Cull(CCamera* cam, CULL_MODE emMode);
			virtual void ConvertFromHomogeneous4D();

			virtual void RemoveBackface(CCamera* cam);
			virtual bool Visible();
		public:
			CDDraw*				m_pDDraw;
			DWORD				m_dwAttribute;
			DWORD				m_dwStatus;
			CMatrix4x4*			m_pCameraMatrix;
			CMatrix4x4*			m_pScreenMatrix;
			CMatrix4x4*			m_pProjectMatrix;
			CVector4D*			m_pWorldPos;
			CCamera*			m_pCamera;
			vector<IPolygon*>*	m_pPolygonList;
			vector<CVector4D*>* m_pLocalList;
			vector<CVector4D*>* m_pTransList;
			CString				m_strName;
			int					m_nNumPolygones;
			int					m_nNumVertices;
			CVector4D			m_vUx, m_vUy, m_vUz;  // local axes to track full orientation
			double				m_fAvgRadius;
			double				m_fMaxRadius;
		};
	}
}