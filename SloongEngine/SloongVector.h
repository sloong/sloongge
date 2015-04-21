#pragma once
#include "SloongMathBase.h"
#include "SloongMatrix.h"
namespace SoaringLoong
{
	namespace Math
	{
		using namespace Matrix;
		namespace Vector
		{
			typedef class SLOONGENGINE_API CVector2D
			{
			public:
				union
				{
					float M[2]; // array indexed storage

					// explicit names
					struct
					{
						float x, y;
					}; // end struct

				}; // end union

			public:
				inline void VECTOR2D_ZERO(CVector2D* v)
				{
					(v)->x = (v)->y = 0.0;
				}
				inline void VECTOR2D_INITXY(CVector2D* v, float x, float y)
				{
					(v)->x = (x); (v)->y = (y);
				}

				// vector intialization macros to initialize with other vectors
				inline void VECTOR2D_INIT(CVector2D* vdst, CVector2D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
				}

				// vector copying macros
				inline void VECTOR2D_COPY(CVector2D* vdst, CVector2D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
				}

				// point initialization macros
				inline void POINT2D_INIT(CVector2D* vdst, CVector2D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
				}

				// point copying macros
				inline void POINT2D_COPY(CVector2D* vdst, CVector2D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;
				}

			public:
				void VECTOR2D_Add(CVector2D* va, CVector2D* vb, CVector2D* vsum);
				CVector2D VECTOR2D_Add(CVector2D* va, CVector2D* vb);
				void VECTOR2D_Sub(CVector2D* va, CVector2D* vb, CVector2D* vdiff);
				CVector2D VECTOR2D_Sub(CVector2D* va, CVector2D* vb);
				void VECTOR2D_Scale(float k, CVector2D* va);
				void VECTOR2D_Scale(float k, CVector2D* va, CVector2D* vscaled);
				float VECTOR2D_Dot(CVector2D* va, CVector2D* vb);
				float VECTOR2D_Length(CVector2D* va);
				float VECTOR2D_Length_Fast(CVector2D* va);
				void VECTOR2D_Normalize(CVector2D* va);
				void VECTOR2D_Normalize(CVector2D* va, CVector2D* vn);
				void VECTOR2D_Build(CVector2D* init, CVector2D* term, CVector2D* result);
				float VECTOR2D_CosTh(CVector2D* va, CVector2D* vb);
				void VECTOR2D_Print(CVector2D* va, char *name);
				int Fast_Distance_2D(int x, int y);
			} VECTOR2D, POINT2D, *VECTOR2D_PTR, *POINT2D_PTR;

			typedef class SLOONGENGINE_API CVector3D : public CMathBase
			{
			public:
				union
				{
					float M[3]; // array indexed storage

					// explicit names
					struct
					{
						float x, y, z;
					}; // end struct

				}; // end union
			public:

				inline void Zero()
				{
					(this)->x = (this)->y = (this)->z = 0.0;
				}

				inline void Initialize( float x, float y, float z)
				{
					(this)->x = (x); (this)->y = (y); (this)->z = (z);
				}


				inline void VECTOR3D_INIT(CVector3D* vdst, CVector3D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;  (vdst)->z = (vsrc)->z;
				}


				inline void POINT3D_INIT(CVector3D* vdst, CVector3D* vsrc)
				{
					(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;  (vdst)->z = (vsrc)->z;
				}

				inline CVector3D& operator= (CVector3D* pSrc)
				{
					(this)->x = (pSrc)->x;
					(this)->y = (pSrc)->y;
					(this)->z = (pSrc)->z;
					return *this;
				}

				inline CVector3D& operator= ( const CVector3D& oSrc)
				{
					(this)->x = (oSrc).x;
					(this)->y = (oSrc).y;
					(this)->z = (oSrc).z;
					return (*this);
				}

			public:
				void VECTOR3D_Add(CVector3D* va, CVector3D* vb, CVector3D* vsum);
				CVector3D VECTOR3D_Add(CVector3D* va, CVector3D* vb);
				void VECTOR3D_Sub(CVector3D* va, CVector3D* vb, CVector3D* vdiff);
				CVector3D VECTOR3D_Sub(CVector3D* va, CVector3D* vb);
				void VECTOR3D_Scale(float k, CVector3D* va);
				void VECTOR3D_Scale(float k, CVector3D* va, CVector3D* vscaled);
				float VECTOR3D_Dot(CVector3D* va, CVector3D* vb);
				void VECTOR3D_Cross(CVector3D* va, CVector3D* vb, CVector3D* vn);
				CVector3D VECTOR3D_Cross(CVector3D* va, CVector3D* vb);
				float Length() const;
				float VECTOR3D_Length_Fast(CVector3D* va);
				void Normalize();
				void Normalize( const CVector3D& va);
				void VECTOR3D_Build(CVector3D* init, CVector3D* term, CVector3D* result);
				float VECTOR3D_CosTh(CVector3D* va, CVector3D* vb);
				void VECTOR3D_Print(CVector3D* va, char *name);
				
			} VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

			typedef class SLOONGENGINE_API CVector4D : public CMathBase
			{
			public:
				union
				{
					float M[4]; // array indexed storage

					// explicit names
					struct
					{
						float x, y, z, w;
					}; // end struct
				}; // end union

				CVector4D(int x, int y, int z, int w);
				CVector4D(int x, int y, int z);
				CVector4D(const CVector4D& vSrc);
				CVector4D(){}
				~CVector4D(){}
			public:
				inline void VECTOR4D_ZERO(CVector4D* v)
				{
					(v)->x = (v)->y = (v)->z = 0.0; (v)->w = 1.0;
				}

				inline void Initialize(float x, float y, float z)
				{
					(this)->x = (x); (this)->y = (y); (this)->z = (z); (this)->w = 1.0;
				}

				// used to convert from 4D homogenous to 4D non-homogenous
				inline void VECTOR4D_DIV_BY_W(CVector4D* v)
				{
					(v)->x /= (v)->w; (v)->y /= (v)->w; (v)->z /= (v)->w;
				}

				inline void VECTOR4D_DIV_BY_W_VECTOR3D(CVector4D* v4, VECTOR3D_PTR v3)
				{
					(v3)->x = (v4)->x / (v4)->w; (v3)->y = (v4)->y / (v4)->w; (v3)->z = (v4)->z / (v4)->w;
				}

				inline void Copy( const CVector4D& vSrc)
				{
					x = (vSrc).x;
					y = (vSrc).y;
					z = (vSrc).z;
					w = (vSrc).w;
				}

				inline CVector4D& operator=(const CVector4D& Src)
				{
					(this)->x = Src.x;
					(this)->y = Src.y;
					(this)->z = Src.z;
					(this)->w = Src.w;
					return (*this);
				}

				inline CVector4D& operator=(CVector4D* pSrc)
				{
					(this)->x = (pSrc)->x;
					(this)->y = (pSrc)->y;
					(this)->z = (pSrc)->z;
					(this)->w = (pSrc)->w;
					return (*this);
				}

			public:
				static CVector4D Add( const CVector4D& va, const CVector4D& vb);
				static CVector4D Multiply(const CVector4D& vector, const CMatrix4x4& matrix);
				static CVector4D Subtract(const CVector4D& va, const CVector4D& vb);
				static CVector4D Cross(const CVector4D& va, const CVector4D& vb);
				static float Dot(const CVector4D& va, const CVector4D& vb);

			public:
				void Add(const CVector4D& va);
				void Multiply(const CMatrix4x4& matrix);
				void Subtract(const CVector4D& va);
				float Dot(const CVector4D& va);
				//void VECTOR4D_Cross(CVector4D* va, CVector4D* vb, CVector4D* vn);

			public:
				
				
				
				void VECTOR4D_Scale(float k, CVector4D* va);
				void VECTOR4D_Scale(float k, CVector4D* va, CVector4D* vscaled);
				
				float VECTOR4D_Length(CVector4D* va);
				float VECTOR4D_Length_Fast(CVector4D* va);
				void VECTOR4D_Normalize(CVector4D* va);
				void VECTOR4D_Normalize(CVector4D* va, CVector4D* vn);
				void VECTOR4D_Build(CVector4D* init, CVector4D* term, CVector4D* result);
				float VECTOR4D_CosTh(CVector4D* va, CVector4D* vb);
				void VECTOR4D_Print(CVector4D* va, char *name);
			} CPoint4D, VECTOR4D, POINT4D, *VECTOR4D_PTR, *LPVECTOR4D, *POINT4D_PTR, *LPPOINT4D;
		}
	}
}