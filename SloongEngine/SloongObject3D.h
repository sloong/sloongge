#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif


#include "ISloongObject.h"
#include "string/string.h"
using namespace Sloong::Universal;
namespace Sloong
{
	namespace Graphics3D
	{
		class CCamera;
		class SLOONGENGINE_API CObject3D
		{
		public:
			CObject3D( CDDraw* pDDraw );
			~CObject3D();

		public:
			virtual void LoadPLGMode(const CString& strFileName);
			virtual void Update();
			virtual void Render();
			virtual void RenderAll(CCamera* cam, CMatrix4x4* mTrans);
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

			virtual void SetCurrentIndex(int index);
			virtual int GetCurrentIndex();
			virtual int AddObject( const CVector4D& vPos, const CVector4D& vScale, const CVector4D& vRot);

			virtual void GetRadius(double& avg, double& max);
		protected:
			CDDraw*				m_pDDraw;
			CMatrix4x4*			m_pCameraMatrix;
			CMatrix4x4*			m_pScreenMatrix;
			CMatrix4x4*			m_pProjectMatrix;
			CCamera*			m_pCamera;
			CString				m_strName;
			int					m_nNumPolygones;
			int					m_nNumVertices;
			int					m_nNumObjects;
			CVector4D			m_vUx, m_vUy, m_vUz;  // local axes to track full orientation
			vector<IPolygon*>*	m_pPolygonList;
			vector<CVector4D*>* m_pLocalList;
			vector<CVector4D*>* m_pTransList;

			vector<DWORD>*		m_dwAttribute;
			vector<DWORD>*		m_dwStatus;
			vector<CVector4D*>*	m_pWorldPosList;
			vector<CVector4D*>*	m_pScaleList;
			vector<CVector4D*>*	m_pRotateList;
			vector<double>*		m_fAvgRadiusList;
			vector<double>*		m_fMaxRadiusList;
			int					m_nCurrent;
		};
	}
}