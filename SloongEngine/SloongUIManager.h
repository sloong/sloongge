#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif


#include "IUniversal.h"
enum UI_EVENT
{
	BUTTON_UP = 0,
	BUTTON_DOWN,
	SELECTION_CHANGED,
	TEXTFIELD_CLICKED,
	KEY_PRESS,
	REENTER_INTERFACE,
	TIMER_EXPIRED,
	ENTER_INTERFACE,
	TEXT_SCROLL_END,
	TEXTFIELD_RETURN,
	HOVER_TIMED_START,
	HOVER_END,
	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_UP,
};

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

			void RunGUI(ctstring& strFileName);
			void SendEvent(UI_EVENT emEvent, float arg1 = 0.0f, float arg2 = 0.0f, float arg3 = 0.0f, float arg4 = 0.0f);

			void CreateGUIItem( const UINT nID, const tstring strType, const vector<tstring>& strTexture);
			void DeleteItem(const UINT nID);
			void MoveItem(const UINT nID, const CRect& rcRect);

		protected:
			map<tstring, CUserInterface*> m_UIMap;
			CUserInterface*	m_pCurrentUI;
			CDDraw*							m_pDDraw;
			CLua*							m_pLua;
			ILogSystem*						m_pLog;
		public:
			void Update();
		};
	}
	
}
