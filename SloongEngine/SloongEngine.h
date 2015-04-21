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

namespace SoaringLoong
{
	class CSize;
	class CRect;

	typedef int(*EventFunc)(int id, UI_EVENT args);

	namespace Graphics
	{
		class CDDraw;
	}
	using namespace Graphics;
	class SLOONGENGINE_API CSloongEngine {
	public:
		CSloongEngine(void);
		virtual ~CSloongEngine();
	
		static CDDraw* GetDraw();
		static bool InRect(const CSize& pos, const CRect& rc);
		static void SendEvent(int id, UI_EVENT args);
		void SetEnentHandler(EventFunc func);
	protected:
		static CSloongEngine* theEngine;
		EventFunc g_EventFunc;
	};

}


extern SLOONGENGINE_API int nSloongEngine;

SLOONGENGINE_API int fnSloongEngine(void);
