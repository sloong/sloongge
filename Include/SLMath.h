#pragma once

#ifdef SLMATH_EXPORTS
#define SLMATH_API __declspec(dllexport)
#define VERSION_NUMBER						1,2,0,60
#define VERSION_FILEVERSION					"1.2.0.60"
#define VERSION_BUILDTIME					"2014/10/29"
#ifdef _DEBUG
#define VERSION_PRODUCTVERSION 				"Ver.1.2 for Debug"
#define	VERSION_FILEDESCRIPTION				"Universal Math Debug Libaray"
#define	VERSION_PRODUCTNAME					"Universal Debug Libaray"
#else
#define VERSION_PRODUCTVERSION 				"Ver.1.2"
#define	VERSION_FILEDESCRIPTION				"Universal Math Libaray"
#define	VERSION_PRODUCTNAME					"Universal Libaray"
#endif // _DEBUG
#define VERSION_INTERNALNAME				"SLMath.dll"
#define VERSION_COMPANYNAME 				"SoaringLoong, Inc."
#define	VERSION_LEGALCOPYRIGHT				"Copyright (C) 2013 SoaringLoong, Inc."
#else
#define SLMATH_API __declspec(dllimport)
#endif

#include <math.h>
#include "IUniversal.h"

#pragma region Math Define
// used to compute the min and max of two expresions
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 
#define MAX(a, b)  (((a) > (b)) ? (b) : (a)) 

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 
// used for swapping algorithm
#define SWAP(a,b,t) {t=a; a=b; b=t;}

#define GET_RADIANS(degree) (float)(degree / 180.0f * PI)
#define MAX_SIDES       6

#define UGP_FRONT    0
#define UGP_BACK     1
#define UGP_ON_PLANE 2
#define UGP_CLIPPED  3
#define UGP_CULLED   4
#define UGP_VISIBLE  5
#pragma endregion

namespace SoaringLoong
{
	namespace SLMath
	{
		// a 2D vertex
		typedef struct VERTEX2DI_TYP
		{
			int x, y; // the vertex
		} VERTEX2DI, *VERTEX2DI_PTR;

		// a 2D vertex
		typedef struct VERTEX2DF_TYP
		{
			float x, y; // the vertex
		} VERTEX2DF, *VERTEX2DF_PTR;


		class CSLoongPolygon2D
		{
		public:
			int state;        // state of polygon
			int num_verts;    // number of vertices
			int x0, y0;        // position of center of polygon  
			int xv, yv;        // initial velocity
			DWORD color;      // could be index or PALETTENTRY
			VERTEX2DF *vlist; // pointer to vertex list
		};

		class CSLoongMatrix3x3
		{
		public:
			union
			{
				float M[3][3]; // array indexed data storage

				// storage in row major form with explicit names
				struct
				{
					float M00, M01, M02;
					float M10, M11, M12;
					float M20, M21, M22;
				}; // end explicit names

			}; // end union
		};

		class CSLoongMatrix1x3
		{
		public:
			union
			{
				float M[3]; // array indexed data storage

				// storage in row major form with explicit names
				struct
				{
					float M00, M01, M02;

				}; // end explicit names
			}; // end union
		};

		class CSLoongMatrix3x2
		{
		public:
			union
			{
				float M[3][2]; // array indexed data storage

				// storage in row major form with explicit names
				struct
				{
					float M00, M01;
					float M10, M11;
					float M20, M21;
				}; // end explicit names

			}; // end union
		};

		class CSLoongMatrix1x2
		{
		public:
			union
			{
				float M[2]; // array indexed data storage

				// storage in row major form with explicit names
				struct
				{
					float M00, M01;

				}; // end explicit names
			}; // end union
		};

		class  CSLMath
		{
			// matrix defines
		public:
			CSLMath();
			~CSLMath();

			void Build_Sin_Cos_Tables(void);

			static int Mat_Mul1X2_3X2(CSLoongMatrix1x2* ma, CSLoongMatrix3x2* mb, CSLoongMatrix1x2* mprod);
			static int Mat_Mul_1X2_3X2(CSLoongMatrix1x2* ma,
				CSLoongMatrix3x2* mb,
				CSLoongMatrix1x2* mprod);

			static int Mat_Mul_1X3_3X3(CSLoongMatrix1x3* ma,
				CSLoongMatrix3x3* mb,
				CSLoongMatrix1x3* mprod);

			static int Mat_Mul_3X3(CSLoongMatrix3x3* ma,
				CSLoongMatrix3x3* mb,
				CSLoongMatrix3x3* mprod);

			static int Mat_Init_3X2(CSLoongMatrix3x2* ma,
				float m00, float m01,
				float m10, float m11,
				float m20, float m21);

			int Translate_Polygon2D_Mat(CSLoongPolygon2D* poly, int dx, int dy);
			int Rotate_Polygon2D_Mat(CSLoongPolygon2D* poly, int theta);
			int Scale_Polygon2D_Mat(CSLoongPolygon2D* poly, float sx, float sy);

			int Translate_Polygon2D(CSLoongPolygon2D* poly, int dx, int dy);
			int Rotate_Polygon2D(CSLoongPolygon2D* poly, int theta);
			int Scale_Polygon2D(CSLoongPolygon2D* poly, float sx, float sy);
			int Fast_Distance_2D(int x, int y);
			float Fast_Distance_3D(float x, float y, float z);


			// storage for our lookup tables
			double cos_look[361]; // 1 extra so we can store 0-360 inclusive
			double sin_look[361]; // 1 extra so we can store 0-360 inclusive

		};

		class SLMATH_API CVector3
		{
		public:
			CVector3();
			CVector3(float X, float Y, float Z);

			void operator=(CVector3 &v);
			CVector3 operator-(CVector3 &v);
			CVector3 operator+(CVector3 &v);
			CVector3 operator*(CVector3 &v);
			CVector3 operator/(CVector3 &v);

			CVector3 operator+(float f);
			CVector3 operator-(float f);
			CVector3 operator*(float f);
			CVector3 operator/(float f);

			void operator +=(CVector3 &v);
			void operator -=(CVector3 &v);
			void operator *=(CVector3 &v);
			void operator /=(CVector3 &v);
			void operator +=(float f);
			void operator -=(float f);
			void operator *=(float f);
			void operator /=(float f);

			void CrossProduct(CVector3 &v1, CVector3 &v2);
			float DotProduct3(CVector3 &v1);
			float GetLength();
			void Normal();
			void Normalize(CVector3 &v1, CVector3 &v2, CVector3 &v3);

			float x, y, z;
		};

		class CPlane;
		class SLMATH_API CRay
		{
		public:
			CRay(){}

			bool Intersect(CVector3 &pos, float radius, float *dist);
			bool Intersect(CVector3 &p1, CVector3 &p2, CVector3 &p3, bool cull, float *dist);
			bool Intersect(CVector3 &p1, CVector3 &p2, CVector3 &p3, bool cull, float length, float *dist);
			bool Intersect(CPlane &pl, bool cull, CVector3 *intersectPoint, float *dist);
			bool Intersect(CPlane &pl, bool cull, float length, CVector3 *intersectPoint, float *dist);

			CVector3 m_origin;
			CVector3 m_direction;
		};


		class SLMATH_API CQuaternion
		{
		public:
			CQuaternion();
			CQuaternion(float xAxis, float yAxis, float zAxis, float wAxis);

			void operator=(const CQuaternion &q);
			CQuaternion operator*(const CQuaternion &q);
			CQuaternion operator+(const CQuaternion &q);

			void CreateQuatFromAxis(CVector3 &a, float radians);

			float Length();
			void Normal();

			CQuaternion Conjugate();
			CQuaternion CrossProduct(const CQuaternion &q);

			void Rotatef(float amount, float xAxis, float yAxis, float zAxis);
			void RotationRadiansf(double X, double Y, double Z);

			void CreateMatrix(float *pMatrix);

			void Slerp(const CQuaternion &q1, const CQuaternion &q2, float t);

			float w, x, y, z;
		};


		class SLMATH_API CPolygon
		{
		public:
			CPolygon() : m_flag(0) { }

			void SetFlag(unsigned int fl) { m_flag = fl; }
			unsigned int GetFlag() { return m_flag; }
			void Copy(CPolygon &pol);

			void Clip(CPlane &pl, CPolygon *front, CPolygon *back);

			bool Intersect(CRay &ray, bool cull, float *dist);

			unsigned int m_flag;
			CVector3 m_vertexList[3];
		};



		class SLMATH_API CPlane
		{
		public:
			CPlane();
			CPlane(float A, float B, float C, float D);
			CPlane(CVector3 &n, float D);

			void CreatePlaneFromTri(CVector3 &v1, CVector3 &v2,
				CVector3 &v3);

			void SetPlaneNormal(float A, float B, float C)
			{
				a = A; b = B; c = C;
			}
			void SetPlaneIntercept(float D) { d = D; }

			bool Intersect(CPlane &p2, CPlane &p3,
				CVector3 *intersectPoint);
			bool Intersect(CPlane &pl, CVector3 *intersectPoint);
			bool Intersect(CPlane &pl, CRay *intersect);
			bool Intersect(CVector3 &p1, CVector3 &p2, CVector3 &p3);

			int ClassifyPolygon(CPolygon &pol);
			int ClassifyPoint(CVector3 &v);
			int ClassifyPoint(float x, float y, float z);

			void Lerp(CPlane &p1, CPlane &p2, float amount);

			float GetDistance(CVector3 &v)
			{
				return a * v.x + b * v.y + c * v.z + d;
			}

			float GetDistance(float x, float y, float z)
			{
				return a * x + b * y + c * z + d;
			}

			float a, b, c, d;
		};



		class SLMATH_API CFrustum
		{
		public:
			void CalculateFrustum(float *mv, float *proj);
			bool isPointVisiable(float x, float y, float z);
			bool isSphereVisiable(float x, float y, float z, float radius);
			bool isBoxVisiable(float x, float y, float z, float size);

		protected:
			void NormalizeFrustum();

		protected:
			CPlane Frustum[MAX_SIDES];
		};


		class SLMATH_API CMatrix4x4
		{
		public:
			CMatrix4x4() { Identity(); }
			CMatrix4x4(const CMatrix4x4 &m);
			CMatrix4x4(float r11, float r12, float r13, float r14,
				float r21, float r22, float r23, float r24,
				float r31, float r32, float r33, float r34,
				float r41, float r42, float r43, float r44);
			~CMatrix4x4() {}

			void Identity();

			void operator=(CMatrix4x4 &m);
			CMatrix4x4 operator-(CMatrix4x4 &m);
			CMatrix4x4 operator+(CMatrix4x4 &m);
			CMatrix4x4 operator*(CMatrix4x4 &m);

			CMatrix4x4 operator*(float f);
			CMatrix4x4 operator/(float f);

			void operator +=(CMatrix4x4 &m);
			void operator -=(CMatrix4x4 &m);
			void operator *=(CMatrix4x4 &m);

			void operator *=(float f);
			void operator /=(float f);

			void Translate(CVector3 &Translate);
			void Translate(float x, float y, float z);
			void inverseTranslate();

			void Rotate(double angle, int x, int y, int z);

			CVector3 VectorMatrixMultiply(CVector3 &v);
			CVector3 VectorMatrixMultiply3x3(CVector3 &v);

			float matrix[4][4];
		};

		class SLMATH_API CMatrix
		{
		public:
			CMatrix();
			CMatrix(UINT unRow, UINT unColumn, double* pValue);
			CMatrix(const CMatrix& oOrg);
			~CMatrix();

			void operator =(CMatrix& m);
			CMatrix operator -(CMatrix& m);
			CMatrix operator +(CMatrix& m);
			CMatrix operator *(CMatrix& m);

			CMatrix operator *(double d);
			CMatrix operator /(double d);

			void operator +=(CMatrix& m);
			void operator -=(CMatrix& m);
			void operator *=(CMatrix& m);

			void operator *=(double f);
			void operator /=(double f);

			void Identity();

			int m_nColumn;
			int m_nRow;
			double** m_pMatrix;
		};
	}
}