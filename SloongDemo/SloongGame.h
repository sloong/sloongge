#pragma once
#include "SloongLua.h"
#include "SloongUIManager.h"
#include "resource.h"
using namespace SoaringLoong;


#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#include "SloongModelLoader.h"
using namespace SoaringLoong::Loader;

namespace SoaringLoong
{
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

	void			Section7Test2Init();
	void			Section7Test2Render();

	void			Section7Test1Init();
	void			Section7Test1Render();

	void			Section7Test3Init();
	void			Section7Test3Render();

	void			Section7Test6Init();
	void			Section7Test6Render();

public:
	static int Version(lua_State* l);
	static int RegisterEvent(lua_State* l);
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

public:
	static int SendEvent(int id, LPCTSTR args);
	static int SendEvent(int id, UI_EVENT event);
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
	static CLua* g_pLua;
	static CSloongGame* g_pApp;
	static CUIManager* g_pUIManager;

public:
	HINSTANCE	m_hInst;
	LPCTSTR		m_strTitle;
	LPCTSTR		m_strWindowClass;
	HWND		m_hMainWnd;
	int			m_nHeight;
	int			m_nWidth;
	CDDraw*		m_pDraw;
	IUniversal*			m_pUniversal;
	ILogSystem*			m_pLog;
	CSloongD3D*			m_pD3D;
	DInputClass*		m_pInput;
	RECT				m_rcWindow;
public:
	// Test
	static RENDERLIST4DV1 rend_list; // the render list
	POINT4D        towers[96],
		tanks[24];

	CAM4DV1        cam;       // the single camera
	CCamera*			m_pCam;

	OBJECT4DV1     obj_tower,    // used to hold the master tower
		obj_tank,     // used to hold the master tank
		obj_marker,   // the ground marker
		obj_player;   // the player object        

	 VECTOR4D vscale , vpos , vrot;
	 //OBJECT4DV1 obj;
	 POLYF4DV1 poly1[4];


};