#pragma once
#include "SloongDraw.h"
using namespace SoaringLoong::Graphics;
#include "SloongVector.h"
using namespace SoaringLoong::Math::Vector;
#include "SloongMatrix.h"
using namespace SoaringLoong::Math::Matrix;

namespace SoaringLoong
{
	namespace Math
	{

	

		// 2D polar coordinates ///////////////////////////////////////
		typedef struct CPolar2D
		{
			float r;     // the radi of the point
			float theta; // the angle in rads
		} POLAR2D, *POLAR2D_PTR;

		// 3D cylindrical coordinates ////////////////////////////////
		typedef struct CCylindrical3D
		{
			float r;     // the radi of the point
			float theta; // the angle in degrees about the z axis
			float z;     // the z-height of the point
		} CYLINDRICAL3D, *CYLINDRICAL3D_PTR;

		// 3D spherical coordinates //////////////////////////////////
		typedef struct CSpherical3D
		{
			float p;      // rho, the distance to the point from the origin
			float theta;  // the angle from the z-axis and the line segment o->p
			float phi;    // the angle from the projection if o->p onto the x-y 
			// plane and the x-axis
		} SPHERICAL3D, *SPHERICAL3D_PTR;


		class CMath2
		{
			// matrix defines
		public:
			CMath2();
			~CMath2();

			static void Build_Sin_Cos_Tables(void);

			static int Mat_Mul1X2_3X2(CMatrix1x2* ma, CMatrix3x2* mb, CMatrix1x2* mprod);
			static int Mat_Mul_1X2_3X2(CMatrix1x2* ma,
				CMatrix3x2* mb,
				CMatrix1x2* mprod);

			static int Mat_Mul_1X3_3X3(CMatrix1x3* ma,
				CMatrix3x3* mb,
				CMatrix1x3* mprod);

			
			static int Translate_Polygon2D_Mat(CPolygon2D* poly, int dx, int dy);
			static int Rotate_Polygon2D_Mat(CPolygon2D* poly, int theta);
			static int Scale_Polygon2D_Mat(CPolygon2D* poly, float sx, float sy);

			static int Translate_Polygon2D(CPolygon2D* poly, int dx, int dy);
			static int Rotate_Polygon2D(CPolygon2D* poly, int theta);
			static int Scale_Polygon2D(CPolygon2D* poly, float sx, float sy);
			
			static float Fast_Sin(float theta);
			static float Fast_Cos(float theta);
		};

		


		// fixed point types //////////////////////////////////////////

		typedef int FIXP16;
		typedef int *FIXP16_PTR;



		// vector macros, note the 4D vector sets w=1
		// vector zeroing macros
		

		

		

		// macros to initialize vectors with explicit components
		

		// quaternion macros
		


		// polar, cylindrical, spherical functions
		void POLAR2D_To_POINT2D(POLAR2D_PTR polar, POINT2D_PTR rect);
		void POLAR2D_To_RectXY(POLAR2D_PTR polar, float *x, float *y);
		void POINT2D_To_POLAR2D(POINT2D_PTR rect, POLAR2D_PTR polar);
		void POINT2D_To_PolarRTh(POINT2D_PTR rect, float *r, float *theta);
		void CYLINDRICAL3D_To_POINT3D(CYLINDRICAL3D_PTR cyl, POINT3D_PTR rect);
		void CYLINDRICAL3D_To_RectXYZ(CYLINDRICAL3D_PTR cyl, float *x, float *y, float *z);
		void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect, CYLINDRICAL3D_PTR cyl);
		void POINT3D_To_CylindricalRThZ(POINT3D_PTR rect, float *r, float *theta, float *z);
		void SPHERICAL3D_To_POINT3D(SPHERICAL3D_PTR sph, POINT3D_PTR rect);
		void SPHERICAL3D_To_RectXYZ(SPHERICAL3D_PTR sph, float *x, float *y, float *z);
		void POINT3D_To_SPHERICAL3D(POINT3D_PTR rect, SPHERICAL3D_PTR sph);
		void POINT3D_To_SphericalPThPh(POINT3D_PTR rect, float *p, float *theta, float *phi);

		// quaternion functions
		

		// 2d parametric line functions
		
		// 3d plane functions
		

		// fixed point functions
		FIXP16 FIXP16_MUL(FIXP16 fp1, FIXP16 fp2);
		FIXP16 FIXP16_DIV(FIXP16 fp1, FIXP16 fp2);
		void FIXP16_Print(FIXP16 fp);


		inline void Write_Error(char* sz, ...){}
	}
	
}
