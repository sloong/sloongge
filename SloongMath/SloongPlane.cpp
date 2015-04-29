#include "stdafx.h"
#include "SloongPlane.h"

using namespace SoaringLoong::Math;

SoaringLoong::Math::CPlane::CPlane()
{
}


SoaringLoong::Math::CPlane::~CPlane()
{
}

void CPlane::Initialize(const POINT3D& p0,const VECTOR3D& normal, bool normalize)
{
	// this function initializes a 3d plane

	// copy the point
	this->p0 = p0;

	// if normalize is 1 then the normal is made into a unit vector
	if (!normalize)
		this->n = normal;
	else
	{
		// make normal into unit vector
		this->n.Normalize(normal);
	} // end else

}

float SoaringLoong::Math::CPlane::ComputePointInPlane(POINT3D_PTR pt)
{
	// test if the point in on the plane, in the positive halfspace
	// or negative halfspace
	float hs = this->n.x*(pt->x - this->p0.x) +
		this->n.y*(pt->y - this->p0.y) +
		this->n.z*(pt->z - this->p0.z);

	// return half space value
	return(hs);
}

INTERSECT_TYPE SoaringLoong::Math::CPlane::Intersect_Parm_Line3D_Plane3D(PARMLINE3D_PTR pline, float *t, POINT3D_PTR pt)
{
	// this function determines where the sent parametric line 
	// intersects the plane the function will project the line 
	// infinitely in both directions, to compute the intersection, 
	// but the line segment defined by p intersected the plane iff t e [0,1]
	// also the function returns 0 for no intersection, 1 for 
	// intersection of the segment and the plane and 2 for intersection 
	// of the line along the segment and the plane 3, the line lies 
	// in the plane

	// first test of the line and the plane are parallel, if so 
	// then they can't intersect unless the line lies in the plane!

	float plane_dot_line = pt->VECTOR3D_Dot(&pline->v, &this->n);

	if (fabs(plane_dot_line) <= EPSILON_E5)
	{
		// the line and plane are co-planer, does the line lie 
		// in the plane?
		if (fabs(ComputePointInPlane(&pline->p0)) <= EPSILON_E5)
			return(PARM_LINE_INTERSECT_EVERYWHERE);
		else
			return(PARM_LINE_NO_INTERSECT);
	} // end if

	// from chapter 4 we know that we can solve for the t where 
	// intersection occurs by
	// a*(x0+vx*t) + b*(y0+vy*t) + c*(z0+vz*t) + d =0
	// t = -(a*x0 + b*y0 + c*z0 + d)/(a*vx + b*vy + c*vz)
	// x0,y0,z0, vx,vy,vz, define the line
	// d = (-a*xp0 - b*yp0 - c*zp0), xp0, yp0, zp0, define the point on the plane 

	*t = -(this->n.x*pline->p0.x +
		this->n.y*pline->p0.y +
		this->n.z*pline->p0.z -
		this->n.x*this->p0.x -
		this->n.y*this->p0.y -
		this->n.z*this->p0.z) / (plane_dot_line);

	// now plug t into the parametric line and solve for x,y,z
	pt->x = pline->p0.x + pline->v.x*(*t);
	pt->y = pline->p0.y + pline->v.y*(*t);
	pt->z = pline->p0.z + pline->v.z*(*t);

	// test interval of t
	if (*t >= 0.0 && *t <= 1.0)
		return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else
		return(PARM_LINE_INTERSECT_OUT_SEGMENT);


}