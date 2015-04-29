#pragma once
#include "SloongVector.h"
#include "SloongParmLine.h"
namespace SoaringLoong
{
	namespace Math
	{
		using namespace Vector;

		typedef class SLOONGMATH_API CPlane
		{
		public:
			CPlane();
			virtual ~CPlane();

			void Initialize(const POINT3D& p0,const VECTOR3D& normal, bool normalize);
			float ComputePointInPlane(POINT3D_PTR pt);
			INTERSECT_TYPE Intersect_Parm_Line3D_Plane3D(PARMLINE3D_PTR pline,float *t, POINT3D_PTR pt);
		public:
			POINT3D p0; // point on the plane
			VECTOR3D n; // normal to the plane (not necessarily a unit vector)

		}PLANE3D, *PLANE3D_PTR;
	}
}
