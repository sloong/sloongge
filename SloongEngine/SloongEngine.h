// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SLOONGENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SLOONGENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif

#include "univ/univ.h"

typedef LPVOID(*pCallBack)(LPVOID);
typedef pCallBack LPCALLBACKFUNC;

namespace Sloong
{
	namespace Graphics
	{
		class CDDraw;
	}
	using namespace Graphics;

	namespace Graphics3D
	{
		class CObject3D;
		class CCamera;
	}
	using namespace Graphics3D;
	using namespace Universal;
	class SLOONGENGINE_API CSloongEngine {
	public:
		CSloongEngine(void);
		virtual ~CSloongEngine();
	
		static CDDraw* GetDraw();
		static bool InRect(const CSize& pos, const CRect& rc);
		static void SendEvent(int id, UI_EVENT args);
		static LPVOID RenderCallBack(LPVOID lpData);
		static void AddRenderTask(CObject3D* pObj,int nIndex, CCamera* pCamera);
		static int GetEventListTotal();
		void SetEnentHandler(LPCALLBACKFUNC func);
	protected:
		static CSloongEngine* theEngine;
		LPCALLBACKFUNC g_EventFunc;
	};

}
