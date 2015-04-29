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
		class SLOONGENGINE_API IObject
		{
		public:
			static IObject* Create3D(CDDraw* pDDraw);

		public:
			virtual void LoadPLGMode(LPCTSTR strFileName, int key, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRot) = 0;
			virtual void Update() = 0;
			virtual void Render() = 0;
			virtual void RenderAll( CCamera* cam, CMatrix4x4* mTrans ) = 0;
			virtual void UpdateWorldVertex(const CVector4D& mWorld) = 0;
			virtual void UpdateCameraVertex(const CMatrix4x4& mCamera) = 0;
			virtual void UpdateProjectVertex(const CMatrix4x4& mProject) = 0;
			virtual void UpdateScreenVertex(const CMatrix4x4& mScreen) = 0;
			virtual void UpdateVertex(const CMatrix4x4& mMarix, bool bNormal) = 0;
			virtual void AddPolygon(IPolygon* pPoly) = 0;
			virtual void AddVertex(const CVector4D& vVertex) = 0;
			virtual CVector4D* GetVertex(int nIndex) = 0;
			virtual void AddAttribute(DWORD dwAttribute) = 0;
			virtual void SetAttribute(DWORD dwAttribute) = 0;
			virtual DWORD GetAttribute() = 0;
			virtual void AddStatus(DWORD dwStatus) = 0;
			virtual void DeleteStatus(DWORD dwStatus) = 0;
			virtual void SetStatus(DWORD dwStatus) = 0;
			virtual DWORD GetStatus() = 0;

			virtual void SetWorldPosition(const CVector4D& vPos) = 0;
			virtual CVector4D GetWorldPosition() = 0;

			virtual void ComputeRadius() = 0;

			virtual void Reset() = 0;

			virtual void Transform(const CMatrix4x4& mMatrix, TRANS_MODE emMode, bool transform_basis) = 0;
			virtual void Move(const CVector4D& vTrans) = 0;
			virtual void Scale(const CVector4D& vScale) = 0;

			virtual void Rotate(float x, float y, float z) = 0;

			virtual void ToWorld(TRANS_MODE emMode) = 0;
			virtual void ToCamera(CCamera* pCam) = 0;
			virtual void ToProject(CCamera* pCam) = 0;
			virtual void CameraToPerspectiveScreen(CCamera* pCam) = 0;
			virtual void PerspectiveToScreen(CCamera* pCam) = 0;
			virtual void Cull(CCamera* cam, CULL_MODE emMode) = 0;
			virtual void ConvertFromHomogeneous4D() = 0;

			virtual void RemoveBackface(CCamera* cam) = 0;
			virtual bool Visible() = 0;

			virtual void AddObject( int key, const CVector4D& vPos) = 0;
			virtual void SetCurrentKey(int key) = 0;
			virtual int GetCurrentKey() = 0;
			virtual int GetCurrentIndex() = 0;

			virtual void GetRadius(double& avg, double& max) = 0;
		};
	}
}