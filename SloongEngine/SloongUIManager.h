#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif


#include "IUniversal.h"

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

			void Initialize( CDDraw* pDDraw, CLua* pLua, ILogSystem* pLog,HWND hWnd);
			void RunGUI(ctstring& strFileName);

			void CreateGUIItem( const UINT nID, const tstring strType, const vector<tstring>& strTexture);
			void DeleteItem(const UINT nID);
			void MoveItem(const UINT nID, const CRect& rcRect);

		protected:
			map<tstring, CUserInterface*>* m_UIMap;
			CUserInterface*	m_pCurrentUI;
			CDDraw*							m_pDDraw;
			CLua*							m_pLua;
			ILogSystem*						m_pLog;
			HWND							m_hWnd;
		public:
			void Update();
		};
	}
	
}
