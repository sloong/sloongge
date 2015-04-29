#pragma once

namespace SoaringLoong
{
	namespace Math
	{
		namespace Vector
		{
			class CVector2D;
		}
		using Vector::CVector2D;

		typedef class CPolar2D
		{
		public:
			CPolar2D();
			virtual ~CPolar2D();

			void POLAR2D_To_POINT2D(CPolar2D* polar, CVector2D* rect);
			void POLAR2D_To_RectXY(CPolar2D* polar, float *x, float *y);
			void POINT2D_To_POLAR2D(CVector2D* rect, CPolar2D* polar);
			void POINT2D_To_PolarRTh(CVector2D* rect, float *r, float *theta);
		public:
			float r;     // the radi of the point
			float theta; // the angle in rads
		}POLAR2D, *POLAR2D_PTR;
	}
}