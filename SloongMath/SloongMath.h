// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SLOONGMATH_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SLOONGMATH_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SLOONGMATH_EXPORTS
#define SLOONGMATH_API __declspec(dllexport)
#else
#define SLOONGMATH_API __declspec(dllimport)
#endif

namespace SoaringLoong
{
	namespace Math
	{
		namespace Vector
		{

		}
	}
}

// This class is exported from the SloongMath.dll
class SLOONGMATH_API CSloongMath {
public:
	CSloongMath(void);
	// TODO: add your methods here.
};

extern SLOONGMATH_API int nSloongMath;

SLOONGMATH_API int fnSloongMath(void);
