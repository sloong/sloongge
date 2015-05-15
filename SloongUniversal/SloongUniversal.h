// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SLOONGUNIVERSAL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SLOONGUNIVERSAL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SLOONGUNIVERSAL_EXPORTS
#define SLOONGUNIVERSAL_API __declspec(dllexport)
#else
#define SLOONGUNIVERSAL_API __declspec(dllimport)
#endif

// This class is exported from the SloongUniversal.dll
class SLOONGUNIVERSAL_API CSloongUniversal {
public:
	CSloongUniversal(void);
	// TODO: add your methods here.
};

extern SLOONGUNIVERSAL_API int nSloongUniversal;

SLOONGUNIVERSAL_API int fnSloongUniversal(void);
