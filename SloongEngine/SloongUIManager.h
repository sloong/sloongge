#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif


#include "IUniversal.h"
#include "SloongVector.h"
#include "SloongObject3D.h"
#include "SloongCamera.h"
#include "DInputClass.h"
using SoaringLoong::Math::Vector::CVector4D;
using SoaringLoong::Graphics3D::IObject;
using SoaringLoong::Graphics3D::CCamera;
namespace SoaringLoong
{
	class CLua;
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

			void Initialize( CDDraw* pDDraw, CLua* pLua, DInputClass* pInput, ILogSystem* pLog,HWND hWnd);
			void RunGUI(ctstring& strFileName);

			void CreateGUIItem( const UINT nID, const tstring strType, const vector<tstring>& strTexture);
			void DeleteItem(const UINT nID);
			void MoveItem(const UINT nID, const CRect& rcRect);

			void Load3DModule(const int& nID, const tstring& strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRotate);
			void Move3DModule(const int& nID, const CVector4D& vPos);
			void Delete3DModule(const int& nID);

			void SetCamera( CCamera* pCamera );
		protected:
			map<tstring, CUserInterface*>*	m_UIMap;
			map<tstring, IObject*>*			m_pModuleMap;
			CUserInterface*					m_pCurrentUI;
			CDDraw*							m_pDDraw;
			CLua*							m_pLua;
			ILogSystem*						m_pLog;
			HWND							m_hWnd;
			CCamera*						m_pCamera;
			DInputClass*					m_pInput;
		public:
			void Update();
		};
	}
	
}
