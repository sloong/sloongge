#include "stdafx.h"
#include "SloongPolar2D.h"
using namespace SoaringLoong::Math;
#include "SloongVector.h"
using SoaringLoong::Math::Vector::CVector2D;

SoaringLoong::Math::CPolar2D::CPolar2D()
{

}

SoaringLoong::Math::CPolar2D::~CPolar2D()
{

}



void SoaringLoong::Math::CPolar2D::POLAR2D_To_POINT2D(POLAR2D_PTR polar, POINT2D_PTR rect)
{
	// convert polar to rectangular
	rect->x = polar->r*cosf(polar->theta);
	rect->y = polar->r*sinf(polar->theta);

} // end POLAR2D_To_POINT2D

////////////////////////////////////////////////////////////////

void SoaringLoong::Math::CPolar2D::POLAR2D_To_RectXY(POLAR2D_PTR polar, float *x, float *y)
{
	// convert polar to rectangular
	*x = polar->r*cosf(polar->theta);
	*y = polar->r*sinf(polar->theta);

} // end POLAR2D_To_RectXY

///////////////////////////////////////////////////////////////

void SoaringLoong::Math::CPolar2D::POINT2D_To_POLAR2D(POINT2D_PTR rect, POLAR2D_PTR polar)
{
	// convert rectangular to polar
	polar->r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	polar->theta = atanf(rect->y / rect->x);

} // end POINT2D_To_POLAR2D

////////////////////////////////////////////////////////////

void SoaringLoong::Math::CPolar2D::POINT2D_To_PolarRTh(POINT2D_PTR rect, float *r, float *theta)
{
	// convert rectangular to polar
	*r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	*theta = atanf(rect->y / rect->x);

} // end POINT2D_To_PolarRTh
