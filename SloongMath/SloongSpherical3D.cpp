#include "stdafx.h"
#include "SloongSpherical3D.h"
using namespace SoaringLoong::Math;
#include "SloongVector.h"
using namespace SoaringLoong::Math::Vector;

SoaringLoong::Math::CSpherical3D::CSpherical3D()
{

}

SoaringLoong::Math::CSpherical3D::~CSpherical3D()
{

}



void SoaringLoong::Math::CSpherical3D::SPHERICAL3D_To_POINT3D(SPHERICAL3D_PTR sph, POINT3D_PTR rect)
{
	// convert spherical to rectangular
	float r;

	// pre-compute r, and z
	r = sph->p*sinf(sph->phi);
	rect->z = sph->p*cosf(sph->phi);

	// use r to simplify computation of x,y
	rect->x = r*cosf(sph->theta);
	rect->y = r*sinf(sph->theta);

} // end SPHERICAL3D_To_POINT3D

////////////////////////////////////////////////////////////////

void SoaringLoong::Math::CSpherical3D::SPHERICAL3D_To_RectXYZ(SPHERICAL3D_PTR sph,
	float *x, float *y, float *z)
{
	// convert spherical to rectangular

	float r;

	// pre-compute r, and z
	r = sph->p*sinf(sph->phi);
	*z = sph->p*cosf(sph->phi);

	// use r to simplify computation of x,y
	*x = r*cosf(sph->theta);
	*y = r*sinf(sph->theta);

} // end SPHERICAL3D_To_RectXYZ

///////////////////////////////////////////////////////////

void SoaringLoong::Math::CSpherical3D::POINT3D_To_SPHERICAL3D(POINT3D_PTR rect, SPHERICAL3D_PTR sph)
{
	// convert rectangular to spherical
	sph->p = sqrtf((rect->x*rect->x) + (rect->y*rect->y) + (rect->z*rect->z));

	sph->theta = atanf(rect->y / rect->x);

	// we need r to compute phi
	float r = sph->p*sinf(sph->phi);

	sph->phi = asinf(r / sph->p);

} // end POINT3D_To_CYLINDRICAL3D

////////////////////////////////////////////////////////////

void SoaringLoong::Math::CSpherical3D::POINT3D_To_SphericalPThPh(POINT3D_PTR rect,
	float *p, float *theta, float *phi)
{
	// convert rectangular to spherical
	*p = sqrtf((rect->x*rect->x) + (rect->y*rect->y) + (rect->z*rect->z));
	*theta = atanf(rect->y / rect->x);

	// we need r to compute phi
	float r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	*phi = asinf(r / (*p));

} // end POINT3D_To_SphericalPThPh