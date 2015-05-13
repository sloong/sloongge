#pragma once
#include "SloongLua.h"
#include "SloongUIManager.h"
#include "resource.h"
using namespace SoaringLoong;
#pragma comment(lib,"SloongEngine.lib")

#include "SloongVector.h"
#include "SloongObject3D.h"
#include "SloongCamera.h"
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Graphics3D;
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

namespace SoaringLoong
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
}
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Graphics3D;
class CSloongD3D;
class DInputClass;
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
	static int SendEvent(int id, LPCTSTR args);
	static   DWORD WINAPI SendEvent(LPVOID pArgs);
	static void ErrorHandler(LPCTSTR strError);

public:
	static LuaFunctionRegistr g_LuaFunctionList[];
	static CLua* GetSloongLua();
	static void SetSloongLua(CLua* pLua);
	static CSloongGame* GetAppMain();
	static void SetAppMain(CSloongGame* pApp);
	static CUIManager* GetSloongUIManager();

	static CDDraw* GetDDraw();
	static ILogSystem* GetLogSystem();
	static IUniversal* GetUniversal();
	static tstring Format(LPCTSTR strText, ...);
	static bool InRect(const CSize& pos, const CRect& rc);

private:
	static CSloongGame* g_pApp;

public:
	HINSTANCE	m_hInst;
	LPCTSTR		m_strTitle;
	LPCTSTR		m_strWindowClass;
	HWND		m_hMainWnd;
	CDDraw*		m_pDraw;
	CUIManager*			m_pUIManager;
	CLua*				m_pLua;
	IUniversal*			m_pUniversal;
	ILogSystem*			m_pLog;
	CSloongD3D*			m_pD3D;
	DInputClass*		m_pInput;
	CRect				m_rcWindow;
	CSloongEngine*		m_pEngine;

	CVector4D vscale, vpos, vrot;
	IObject*     obj_tower,    // used to hold the master tower
		*obj_tank,     // used to hold the master tank
		*obj_marker,   // the ground marker
		*obj_player;   // the player object        

	CVector4D        towers[96], tanks[24];
	CCamera        m_cam;       // the single camera
};