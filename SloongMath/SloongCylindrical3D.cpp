#include "stdafx.h"
#include "SloongCylindrical3D.h"
using namespace SoaringLoong::Math;
#include "SloongVector.h"
using SoaringLoong::Math::Vector::CVector3D;

SoaringLoong::Math::CCylindrical3D::CCylindrical3D()
{

}

SoaringLoong::Math::CCylindrical3D::~CCylindrical3D()
{

}

///////////////////////////////////////////////////////////////

void CYLINDRICAL3D_To_POINT3D(CYLINDRICAL3D_PTR cyl, CVector3D* rect)
{
	// convert cylindrical to rectangular
	rect->x = cyl->r*cosf(cyl->theta);
	rect->y = cyl->r*sinf(cyl->theta);
	rect->z = cyl->z;

} // end CYLINDRICAL3D_To_POINT3D

////////////////////////////////////////////////////////////////

void CYLINDRICAL3D_To_RectXYZ(CYLINDRICAL3D_PTR cyl,
	float *x, float *y, float *z)
{
	// convert cylindrical to rectangular
	*x = cyl->r*cosf(cyl->theta);
	*y = cyl->r*sinf(cyl->theta);
	*z = cyl->z;

} // end CYLINDRICAL3D_To_RectXYZ

///////////////////////////////////////////////////////////////

void POINT3D_To_CYLINDRICAL3D(POINT3D_PTR rect,
	CYLINDRICAL3D_PTR cyl)
{
	// convert rectangular to cylindrical
	cyl->r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	cyl->theta = atanf(rect->y / rect->x);
	cyl->z = rect->z;

} // end POINT3D_To_CYLINDRICAL3D

///////////////////////////////////////////////////////////////

void POINT3D_To_CylindricalRThZ(POINT3D_PTR rect,
	float *r, float *theta, float *z)
{
	// convert rectangular to cylindrical
	*r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	*theta = atanf(rect->y / rect->x);
	*z = rect->z;

} // end POINT3D_To_CylindricalRThZ

///////////////////////////////////////////////////////////////
