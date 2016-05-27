#pragma once
#include "univ/lua.h"
#include "sloongge/SloongUIManager.h"
#include "resource.h"
using namespace Sloong;
#pragma comment(lib,"sloongge.lib")

#include "math/SloongVector.h"
#include "sloongge/SloongObject3D.h"
#include "sloongge/SloongCamera.h"
using namespace Sloong::Math::Vector;
using namespace Sloong::Graphics3D;
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

typedef struct EVENT_PARAM
{
	int id;
	UI_EVENT event;
}*LPEVENT_PARAM;

namespace Sloong
{
	class CSloongEngine;
	namespace Graphics
	{
		class CDDraw;
		class CSprite;
	}
	namespace Graphics3D
	{
		class CCamera;
	}
	namespace Universal
	{
		class CLua;
		class CRect;
		class CSize;
	}
	namespace DirectX
	{
		class CD3D;
		class CDInput;
	}
}
using namespace Sloong;
using namespace Sloong::Graphics;
using namespace Sloong::Graphics3D;
using namespace Sloong::Universal;
using namespace Sloong::DirectX;

class CSloongGame
{
public:
	CSloongGame();
	~CSloongGame();

public:
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

public:
	BOOL			InitInstance(HINSTANCE, int);
	ATOM			MyRegisterClass(const HINSTANCE hInstance);
	int				Run();
	void			Shutdown();
	void			Render();
	void			RenderTest();
	void			InitTest();
public:
	static int Version(lua_State* l);
	static int RegisterEvent(lua_State* l);
	static int RegisterKeyboardEvent(lua_State* l);
	static int CreateGUIItem(lua_State* l);
	static int DeleteGUIItem(lua_State* l);
	static int MoveGUIItem(lua_State* l);
	static int SendEvent(lua_State* l);
	static int RunItemCommand(lua_State* l);
	static int RunGUI(lua_State* l);
	static int SetItemFont(lua_State* l);
	static int EnableItem(lua_State* l);
	static int StartTimer(lua_State* l);
	static int GetMousePos(lua_State* l);
	static int Exit(lua_State* l);
	static int Load3DModule(lua_State* l);
	static int CreateCamera(lua_State* l);
	static int MoveCamera(lua_State* l);

public:
	static int SendEvent(int id, string args);
	static LPVOID SendEvent(LPVOID pArgs);
	static void ErrorHandler(string strError);

public:
	static LuaFunctionRegistr g_LuaFunctionList[];
	static CLua* GetSloongLua();
	static void SetSloongLua(CLua* pLua);
	static CSloongGame* GetAppMain();
	static void SetAppMain(CSloongGame* pApp);
	static CUIManager* GetSloongUIManager();

	static CDDraw* GetDDraw();
	static CLog* GetLogSystem();
	//static IUniversal* GetUniversal();
	static tstring Format(LPCTSTR strText, ...);
	static bool InRect(const CSize& pos, const CRect& rc);

private:
	static CSloongGame* g_pApp;
	mutex m_oEventMutex;

public:
	HINSTANCE	m_hInst;
	LPCTSTR		m_strTitle;
	LPCTSTR		m_strWindowClass;
	HWND		m_hMainWnd;
	CDDraw*		m_pDraw;
	CUIManager*			m_pUIManager;
	CLua*				m_pLua;
//	IUniversal*			m_pUniversal;
	CLog*			m_pLog;
	CD3D*			m_pD3D;
	CDInput*		m_pInput;
	CRect				m_rcWindow;
	CSloongEngine*		m_pEngine;

	CVector4D vscale, vpos, vrot;
	CObject3D*     obj_tower,    // used to hold the master tower
		*obj_tank,     // used to hold the master tank
		*obj_marker,   // the ground marker
		*obj_player;   // the player object        

	CVector4D        towers[96], tanks[24];
	CCamera        m_cam;       // the single camera
};