#include "stdafx.h"
#include "SloongParmLine.h"

using namespace SoaringLoong::Math::Vector;
void SoaringLoong::Math::CParamLine2D::Init_Parm_Line2D(POINT2D_PTR p_init, POINT2D_PTR p_term, PARMLINE2D_PTR p)
{
// this initializes a parametric 2d line, note that the function
	// computes v=p_pend - p_init, thus when t=0 the line p=p0+v*t = p0
	// and whne t=1, p=p1, this way the segement is traced out from
	// p0 to p1 via 0<= t <= 1

	// start point
	p_init->VECTOR2D_INIT(&(p->p0), p_init);

	// end point
	p_term->VECTOR2D_INIT(&(p->p1), p_term);

	// now compute direction vector from p0->p1
	p_init->VECTOR2D_Build(p_init, p_term, &(p->v));

}

void SoaringLoong::Math::CParamLine2D::Compute_Parm_Line2D(PARMLINE2D_PTR p, float t, POINT2D_PTR pt)
{
	// this function computes the value of the sent parametric 
	// line at the value of t

	pt->x = p->p0.x + p->v.x*t;
	pt->y = p->p0.y + p->v.y*t;
}

int SoaringLoong::Math::CParamLine2D::Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, float *t1, float *t2)
{
	// this function computes the interesection of the two parametric 
	// line segments the function returns true if the segments 
	// interesect and sets the values of t1 and t2 to the t values that 
	// the intersection occurs on the lines p1 and p2 respectively, 
	// however, the function may send back t value not in the range [0,1]
	// this means that the segments don't intersect, but the lines that 
	// they represent do, thus a retun of 0 means no intersection, a 
	// 1 means intersection on the segments and a 2 means the lines 
	// intersect, but not necessarily the segments and 3 means that 
	// the lines are the same, thus they intersect everywhere

	// basically we have a system of 2d linear equations, we need
	// to solve for t1, t2 when x,y are both equal (if that's possible)

	// step 1: test for parallel lines, if the direction vectors are 
	// scalar multiples then the lines are parallel and can't possible
	// intersect unless the lines overlap

	float det_p1p2 = (p1->v.x*p2->v.y - p1->v.y*p2->v.x);
	if (fabs(det_p1p2) <= EPSILON_E5)
	{
		// at this point, the lines either don't intersect at all
		// or they are the same lines, in which case they may intersect
		// at one or many points along the segments, at this point we 
		// will assume that the lines don't intersect at all, but later
		// we may need to rewrite this function and take into 
		// consideration the overlap and singular point exceptions
		return(PARM_LINE_NO_INTERSECT);

	} // end if

	// step 2: now compute the t1, t2 values for intersection
	// we have two lines of the form
	// p    = p0    +  v*t, specifically
	// p1   = p10   + v1*t1

	// p1.x = p10.x + v1.x*t1
	// p1.y = p10.y + v1.y*t1

	// p2 = p20 + v2*t2
	// p2   = p20   + v2*t2

	// p2.x = p20.x + v2.x*t2
	// p2.y = p20.y + v2.y*t2
	// solve the system when x1 = x2 and y1 = y2
	// explained in chapter 4
	*t1 = (p2->v.x*(p1->p0.y - p2->p0.y) - p2->v.y*(p1->p0.x - p2->p0.x))
		/ det_p1p2;

	*t2 = (p1->v.x*(p1->p0.y - p2->p0.y) - p1->v.y*(p1->p0.x - p2->p0.x))
		/ det_p1p2;

	// test if the lines intersect on the segments
	if ((*t1 >= 0) && (*t1 <= 1) && (*t2 >= 0) && (*t2 <= 1))
		return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else
		return(PARM_LINE_INTERSECT_OUT_SEGMENT);
}

int SoaringLoong::Math::CParamLine2D::Intersect_Parm_Lines2D(PARMLINE2D_PTR p1, PARMLINE2D_PTR p2, POINT2D_PTR pt)
{
	// this function computes the interesection of the two 
	// parametric line segments the function returns true if 
	// the segments interesect and sets the values of pt to the 
	// intersection point, however, the function may send back a 
	// value not in the range [0,1] on the segments this means 
	// that the segments don't intersect, but the lines that 
	// they represent do, thus a retun of 0 means no intersection, 
	// a 1 means intersection on the segments and a 2 means 
	// the lines intersect, but not necessarily the segments

	// basically we have a system of 2d linear equations, we need
	// to solve for t1, t2 when x,y are both equal (if that's possible)

	// step 1: test for parallel lines, if the direction vectors are 
	// scalar multiples then the lines are parallel and can't possible
	// intersect

	float t1, t2, det_p1p2 = (p1->v.x*p2->v.y - p1->v.y*p2->v.x);

	if (fabs(det_p1p2) <= EPSILON_E5)
	{
		// at this point, the lines either don't intersect at all
		// or they are the same lines, in which case they may intersect
		// at one or many points along the segments, at this point we 
		// will assume that the lines don't intersect at all, but later
		// we may need to rewrite this function and take into 
		// consideration the overlap and singular point exceptions
		return(PARM_LINE_NO_INTERSECT);

	} // end if

	// step 2: now compute the t1, t2 values for intersection
	// we have two lines of the form
	// p    = p0    +  v*t, specifically
	// p1   = p10   + v1*t1

	// p1.x = p10.x + v1.x*t1
	// p1.y = p10.y + v1.y*t1

	// p2 = p20 + v2*t2
	// p2   = p20   + v2*t2

	// p2.x = p20.x + v2.x*t2
	// p2.y = p20.y + v2.y*t2
	// solve the system when x1 = x2 and y1 = y2
	// explained in chapter 4
	t1 = (p2->v.x*(p1->p0.y - p2->p0.y) - p2->v.y*(p1->p0.x - p2->p0.x))
		/ det_p1p2;

	t2 = (p1->v.x*(p1->p0.y - p2->p0.y) - p1->v.y*(p1->p0.x - p2->p0.x))
		/ det_p1p2;

	// now compute point of intersection
	pt->x = p1->p0.x + p1->v.x*t1;
	pt->y = p1->p0.y + p1->v.y*t1;

	// test if the lines intersect on the segments
	if ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1))
		return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else
		return(PARM_LINE_INTERSECT_OUT_SEGMENT);

}

void SoaringLoong::Math::CParamLine3D::Init_Parm_Line3D(POINT3D_PTR p_init, POINT3D_PTR p_term, PARMLINE3D_PTR p)
{
	// this initializes a parametric 3d line, note that the function
	// computes v=p_pend - p_init, thus when t=0 the line p=p0+v*t = p0
	// and whne t=1, p=p1, this way the segement is traced out from
	// p0 to p1 via 0<= t <= 1

	// start point
	p_init->VECTOR3D_INIT(&(p->p0), p_init);

	// end point
	p_term->VECTOR3D_INIT(&(p->p1), p_term);

	// now compute direction vector from p0->p1
	p_init->VECTOR3D_Build(p_init, p_term, &(p->v));
}

void SoaringLoong::Math::CParamLine3D::Compute_Parm_Line3D(PARMLINE3D_PTR p, float t, POINT3D_PTR pt)
{
	// this function computes the value of the sent parametric 
	// line at the value of t

	pt->x = p->p0.x + p->v.x*t;
	pt->y = p->p0.y + p->v.y*t;
	pt->z = p->p0.z + p->v.z*t;
}
