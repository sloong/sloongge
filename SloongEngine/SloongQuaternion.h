#pragma once
#include "SloongMathBase.h"
#include "SloongVector.h"
namespace SoaringLoong
{
	namespace Math
	{
		using namespace Vector;
		namespace Quaternion
		{
			typedef class CQuaternion : public CMathBase
			{
			public:
				CQuaternion(){}
				~CQuaternion(){}

				union
				{
					float M[4]; // array indexed storage w,x,y,z order

					// vector part, real part format
					struct
					{
						float    q0;  // the real part
						VECTOR3D qv;  // the imaginary part xi+yj+zk
					};
					struct
					{
						float w, x, y, z;
					};
				}; // end union

			public:
				inline void QUAT_ZERO(CQuaternion* q)
				{
					(q)->x = (q)->y = (q)->z = (q)->w = 0.0;
				}

				inline void QUAT_INITWXYZ(CQuaternion* q, float w, float x, float y, float z)
				{
					(q)->w = (w); (q)->x = (x); (q)->y = (y); (q)->z = (z);
				}

				inline void QUAT_INIT_VECTOR3D(CQuaternion* q, VECTOR3D_PTR v)
				{
					(q)->w = 0; (q)->x = (v->x); (q)->y = (v->y); (q)->z = (v->z);
				}

				inline void QUAT_INIT(CQuaternion* qdst, CQuaternion* qsrc)
				{
					(qdst)->w = (qsrc)->w;  (qdst)->x = (qsrc)->x;
					(qdst)->y = (qsrc)->y;  (qdst)->z = (qsrc)->z;
				}

				inline void QUAT_COPY(CQuaternion* qdst, CQuaternion* qsrc)
				{
					(qdst)->x = (qsrc)->x; (qdst)->y = (qsrc)->y;
					(qdst)->z = (qsrc)->z; (qdst)->w = (qsrc)->w;
				}

			public:


				void QUAT_Add(CQuaternion* q1, CQuaternion* q2, CQuaternion* qsum);
				void QUAT_Sub(CQuaternion* q1, CQuaternion* q2, CQuaternion* qdiff);
				void QUAT_Conjugate(CQuaternion* q, CQuaternion* qconj);
				void QUAT_Scale(CQuaternion* q, float scale, CQuaternion* qs);
				void QUAT_Scale(CQuaternion* q, float scale);
				float QUAT_Norm(CQuaternion* q);
				float QUAT_Norm2(CQuaternion* q);
				void QUAT_Normalize(CQuaternion* q, CQuaternion* qn);
				void QUAT_Normalize(CQuaternion* q);
				void QUAT_Unit_Inverse(CQuaternion* q, CQuaternion* qi);
				void QUAT_Unit_Inverse(CQuaternion* q);
				void QUAT_Inverse(CQuaternion* q, CQuaternion* qi);
				void QUAT_Inverse(CQuaternion* q);
				void QUAT_Mul(CQuaternion* q1, CQuaternion* q2, CQuaternion* qprod);
				void QUAT_Triple_Product(CQuaternion* q1, CQuaternion* q2, CQuaternion* q3, CQuaternion* qprod);
				void VECTOR3D_Theta_To_QUAT(CQuaternion* q, VECTOR3D_PTR v, float theta);
				void VECTOR4D_Theta_To_QUAT(CQuaternion* q, VECTOR4D_PTR v, float theta);
				void EulerZYX_To_QUAT(CQuaternion* q, float theta_z, float theta_y, float theta_x);
				void QUAT_To_VECTOR3D_Theta(CQuaternion* q, VECTOR3D_PTR v, float *theta);
				void QUAT_Print(CQuaternion* q, char *name);
			}QUAT, *QUAT_PTR, *LPQUAT;
		}
	}
}
