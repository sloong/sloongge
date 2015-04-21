#pragma once
#include "SloongVector.h"
namespace SoaringLoong
{
	namespace Math
	{
		using namespace Vector;

		typedef enum Param_Line_Intersect_Type
		{
			PARM_LINE_NO_INTERSECT,
			PARM_LINE_INTERSECT_IN_SEGMENT,
			PARM_LINE_INTERSECT_OUT_SEGMENT,
			PARM_LINE_INTERSECT_EVERYWHERE,
		} INTERSECT_TYPE;

		// 2D parametric line /////////////////////////////////////////
		typedef class SLOONGENGINE_API CParamLine2D
		{
		public:
			void Init_Parm_Line2D(POINT2D_PTR p_init, POINT2D_PTR p_term, CParamLine2D* p);
			void Compute_Parm_Line2D(CParamLine2D* p, float t, POINT2D_PTR pt);
			int Intersect_Parm_Lines2D(CParamLine2D* p1, CParamLine2D* p2, float *t1, float *t2);
			int Intersect_Parm_Lines2D(CParamLine2D* p1, CParamLine2D* p2, POINT2D_PTR pt);
			
		public:
			POINT2D  p0; // start point of parametric line
			POINT2D  p1; // end point of parametric line
			VECTOR2D v;  // direction vector of line segment
			// |v|=|p0->p1|
		} PARMLINE2D, *PARMLINE2D_PTR;

		// 3D parametric line /////////////////////////////////////////
		typedef class SLOONGENGINE_API CParamLine3D
		{
		public:
			void Init_Parm_Line3D(POINT3D_PTR p_init, POINT3D_PTR p_term, CParamLine3D* p);
			void Compute_Parm_Line3D(CParamLine3D* p, float t, POINT3D_PTR pt);

		public:
			POINT3D  p0; // start point of parametric line
			POINT3D  p1; // end point of parametric line
			VECTOR3D v;  // direction vector of line segment
			// |v|=|p0->p1|
		} PARMLINE3D, *PARMLINE3D_PTR;
	}
}