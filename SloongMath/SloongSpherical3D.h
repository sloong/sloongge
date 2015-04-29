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
		typedef class CSpherical3D
		{
		public:
			CSpherical3D();
			virtual ~CSpherical3D();

			void SPHERICAL3D_To_POINT3D(CSpherical3D* sph, CVector3D* rect);
			void SPHERICAL3D_To_RectXYZ(CSpherical3D* sph, float *x, float *y, float *z);
			void POINT3D_To_SPHERICAL3D(CVector3D* rect, CSpherical3D* sph);
			void POINT3D_To_SphericalPThPh(CVector3D* rect, float *p, float *theta, float *phi);

		public:
			float p;      // rho, the distance to the point from the origin
			float theta;  // the angle from the z-axis and the line segment o->p
			float phi;    // the angle from the projection if o->p onto the x-y 
			// plane and the x-axis
		}SPHERICAL3D, *SPHERICAL3D_PTR;
	}
}
