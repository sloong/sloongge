#pragma once

namespace SoaringLoong
{
	namespace Math
	{
		namespace Vector
		{
			class CVector3D;
		}
		using Vector::CVector3D;
		typedef class CCylindrical3D
		{
		public:
			CCylindrical3D();
			virtual ~CCylindrical3D();

			void CYLINDRICAL3D_To_POINT3D(CCylindrical3D* cyl, CVector3D* rect);
			void CYLINDRICAL3D_To_RectXYZ(CCylindrical3D* cyl, float *x, float *y, float *z);
			void POINT3D_To_CYLINDRICAL3D(CVector3D* rect, CCylindrical3D* cyl);
			void POINT3D_To_CylindricalRThZ(CVector3D* rect, float *r, float *theta, float *z);
		public:
			float r;     // the radi of the point
			float theta; // the angle in degrees about the z axis
			float z;     // the z-height of the point
		}CYLINDRICAL3D, *CYLINDRICAL3D_PTR;
	}
}