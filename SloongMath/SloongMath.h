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
		class SLOONGMATH_API IMathBase
		{
		public:
			static float Fast_Distance_3D(float x, float y, float z);
			static float Fast_Sin(float theta);
			static float Fast_Cos(float theta);
		};
	}
}