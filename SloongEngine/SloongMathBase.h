#pragma once

namespace SoaringLoong
{
	namespace Math
	{
		inline double AngleToRadian(double angle)
		{
			return angle * PI / 180.0;
		}

		inline double RadianToAngle(double radian)
		{
			return radian * 180.0 / PI;
		}

		class SLOONGENGINE_API CMathBase
		{
		public:
			CMathBase();
			~CMathBase(){}

			float Fast_Distance_3D(float x, float y, float z);

			static void BuildFastTable();
			static float Fast_Sin(float theta);
			static float Fast_Cos(float theta);

			static float cos_look[361]; // 1 extra so we can store 0-360 inclusive
			static float sin_look[361]; // 1 extra so we can store 0-360 inclusive
			static bool isBuildFastTable;
		};


	}
}
