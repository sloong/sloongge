#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif


#include "univ\\univ.h"
#include "math\\SloongVector.h"
#include "univ\\log.h"
#include "univ/lua.h"
#include "SloongObject3D.h"
#include "SloongCamera.h"
#include "SloongDInput.h"
using Sloong::Math::Vector::CVector4D;
using Sloong::Graphics3D::CObject3D;
using Sloong::Graphics3D::CCamera;
using Sloong::DirectX::CDInput;
namespace Sloong
{
	namespace Graphics
	{
		class CDDraw;
		class CUserInterface;
		class SLOONGENGINE_API CUIManager
		{
		public:
			CUIManager();
			~CUIManager();

		public:
			CUserInterface* GetCurrentUI() const;

			void Initialize( CDDraw* pDDraw, CLua* pLua, CDInput* pInput, CLog* pLog,HWND hWnd);
			void RunGUI(const CString& strFileName);

			void CreateGUIItem( const UINT nID, const CString& strType, vector<CString>* strTexture);
			void DeleteItem(const UINT nID);
			void MoveItem(const UINT nID, const CRect& rcRect);

			void Load3DModule(const int& nID, const CString& strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRotate);
			void Move3DModule(const int& nID, const CVector4D& vPos);
			void Delete3DModule(const int& nID);

			void SetCamera( CCamera* pCamera );
			void MoveCamera(const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target);
		protected:
			map<CString, CUserInterface*>*	m_UIMap;
			map<CString, CObject3D*>*		m_pModuleMap;
			CUserInterface*					m_pCurrentUI;
			CDDraw*							m_pDDraw;
			CLua*							m_pLua;
			CLog*							m_pLog;
			HWND							m_hWnd;
			CCamera*						m_pCamera;
			CDInput*						m_pInput;
		public:
			void Update();
		};
	}
	
}
