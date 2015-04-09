#pragma once

#ifdef SLOONGMATH_EXPORTS
#define SLOONGMATH_API __declspec(dllexport)
#define VERSION_NUMBER						1,1,1,6
#define VERSION_FILEVERSION					"1.1.1.6"
#define VERSION_BUILDTIME					"2014/4/9"
#ifdef _DEBUG
#define VERSION_PRODUCTVERSION 				"Ver.1.1 for Debug"
#define	VERSION_FILEDESCRIPTION				"Universal Math Debug Libaray"
#define	VERSION_PRODUCTNAME					"Universal Debug Libaray"
#else
#define VERSION_PRODUCTVERSION 				"Ver.1.1"
#define	VERSION_FILEDESCRIPTION				"Universal Math Libaray"
#define	VERSION_PRODUCTNAME					"Universal Libaray"
#endif // _DEBUG
#define VERSION_INTERNALNAME				"SLoongMath.dll"
#define VERSION_COMPANYNAME 				"SoaringLoong, Inc."
#define	VERSION_LEGALCOPYRIGHT				"Copyright (C) 2013 SoaringLoong, Inc."
#else
#define SLOONGMATH_API __declspec(dllimport)
#endif

#include <math.h>

namespace SLoongMath
{
#ifndef PI
#define PI 3.14159265358979323846
#endif

#define GET_RADIANS(degree) (float)(degree / 180.0f * PI)
#define MAX_SIDES       6

#define UGP_FRONT    0
#define UGP_BACK     1
#define UGP_ON_PLANE 2
#define UGP_CLIPPED  3
#define UGP_CULLED   4
#define UGP_VISIBLE  5

	class SLOONGMATH_API CVector3
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
	class SLOONGMATH_API CRay
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


	class SLOONGMATH_API CQuaternion 
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


	class SLOONGMATH_API CPolygon
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



	class SLOONGMATH_API CPlane
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



	class SLOONGMATH_API CFrustum
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


	class SLOONGMATH_API CMatrix4x4
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

	class SLOONGMATH_API CMatrix
	{
	public:
		CMatrix();
		CMatrix( UINT unRow, UINT unLine, double* pValue );
		CMatrix( const CMatrix& oOrg );
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

		int m_nRow;
		int m_nLine;
		double** m_pMatrix;
	};
}