#include "stdafx.h"
#include <math.h>
#include "SloongMath2.h"

using namespace SoaringLoong;

CMath2::CMath2()
{
}


CMath2::~CMath2()
{
}


/////////////////////////////////////////////////////////////////

int CMath2::Mat_Mul1X2_3X2(CMatrix1x2* ma,
	CMatrix3x2* mb,
	CMatrix1x2* mprod)
{
	// this function multiplies a 1x2 matrix against a 
	// 3x2 matrix - ma*mb and stores the result
	// using a dummy element for the 3rd element of the 1x2 
	// to make the matrix multiply valid i.e. 1x3 X 3x2

	for (int col = 0; col < 2; col++)
	{
		// compute dot product from row of ma 
		// and column of mb

		double sum = 0; // used to hold result
		int index;
		for (index = 0; index < 2; index++)
		{
			// add in next product pair
			sum += (ma->M[index] * mb->M[index][col]);
		} // end for index

		// add in last element * 1 
		sum += mb->M[index][col];

		// insert resulting col element
		mprod->M[col] = sum;

	} // end for col

	return(1);

} // end Mat_Mul_1X2_3X2


///////////////////////////////////////////////////////////////

// these are the matrix versions, note they are more inefficient for
// single transforms, but their power comes into play when you concatenate
// multiple transformations, not to mention that all transforms are accomplished
// with the same code, just the matrix differs

int CMath2::Translate_Polygon2D_Mat(CPolygon2D* poly, int dx, int dy)
{
	// this function translates the center of a polygon by using a matrix multiply
	// on the the center point, this is incredibly inefficient, but for educational purposes
	// if we had an object that wasn't in local coordinates then it would make more sense to
	// use a matrix, but since the origin of the object is at x0,y0 then 2 lines of code can
	// translate, but lets do it the hard way just to see :)

	// test for valid pointer
	if (!poly)
		return(0);

	CMatrix3x2 mt; // used to hold translation transform matrix

	// initialize the matrix with translation values dx dy
	mt.Mat_Init_3X2(&mt, 1, 0, 0, 1, dx, dy);

	// create a 1x2 matrix to do the transform
	CMatrix1x2 p0 = { poly->x0, poly->y0 };
	CMatrix1x2 p1 = { 0, 0 }; // this will hold result

	// now translate via a matrix multiply
	CMath2::Mat_Mul_1X2_3X2(&p0, &mt, &p1);

	// now copy the result back into polygon
	poly->x0 = p1.M[0];
	poly->y0 = p1.M[1];
	/*
	double lfTemp[] = { 1, 0, 0, 1, dx, dy };
	CMatrix oMatrix3x2(3, 2, lfTemp);

	// create a 1x2 matrix to do the transform
	double lf1x2[] = { poly->x0, poly->y0, 0 };
	CMatrix oMatix1x2(1, 3, lf1x2);
	// now translate via a matrix multiply
	CMatrix oRes = oMatix1x2*oMatrix3x2;
	// now copy the result back into polygon
	poly->x0 = (int)oRes.m_pMatrix[0][0];
	poly->y0 = (int)oRes.m_pMatrix[0][1];*/


	// return success
	return(1);

} // end Translate_Polygon2D_Mat

///////////////////////////////////////////////////////////////

int CMath2::Rotate_Polygon2D_Mat(CPolygon2D* poly, int theta)
{
	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// test for negative rotation angle
	if (theta < 0)
		theta += 360;

	CMatrix3x2 mr; // used to hold rotation transform matrix

	// initialize the matrix with translation values dx dy
	mr.Mat_Init_3X2(&mr, CMathBase::cos_look[theta], CMathBase::sin_look[theta],
		-CMathBase::sin_look[theta], CMathBase::cos_look[theta],
		0, 0);

	// loop and rotate each point, very crude, no lookup!!!
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// create a 1x2 matrix to do the transform
		CMatrix1x2 p0 = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y };
		CMatrix1x2 p1 = { 0, 0 }; // this will hold result

		// now rotate via a matrix multiply
		Mat_Mul_1X2_3X2(&p0, &mr, &p1);

		// now copy the result back into vertex
		poly->vlist[curr_vert].x = p1.M[0];
		poly->vlist[curr_vert].y = p1.M[1];

	} // end for curr_vert

	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	// 	if (!poly)
	// 		return(0);
	// 
	// 	// test for negative rotation angle
	// 	if (theta < 0)
	// 		theta += 360;
	// 
	// 	try
	// 	{
	// 		// used to hold rotation transform matrix
	// 		/*	// initialize the matrix with translation values dx dy
	// 		double lfTemp[] = { cos_look[theta], sin_look[theta],
	// 		-sin_look[theta], cos_look[theta],
	// 		0, 0 };
	// 		CMatrix oMatrix3x2(3, 2, lfTemp);
	// 
	// 		// loop and rotate each point, very crude, no lookup!!!
	// 		for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	// 		{
	// 		// create a 1x2 matrix to do the transform
	// 		double lf1x2[] = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y, 0 };
	// 		CMatrix oMatix1x2(1, 3, lf1x2);
	// 
	// 		// now rotate via a matrix multiply
	// 		//Mat_Mul1X2_3X2(&p0, &mr, &p1);
	// 		CMatrix oRes = oMatix1x2*oMatrix3x2;
	// 
	// 		// now copy the result back into vertex
	// 		poly->vlist[curr_vert].x = oRes.m_pMatrix[0][0];
	// 		poly->vlist[curr_vert].y = oRes.m_pMatrix[0][1];
	// 
	// 		} // end for curr_vert*/
	// 	}
	// 	catch (LPCTSTR strErr)
	// 	{
	// 		//g_oLog.ResLog(ERR,1,strErr);
	// 	}
	// return success
	return(1);

} // end Rotate_Polygon2D_Mat

////////////////////////////////////////////////////////

int CMath2::Scale_Polygon2D_Mat(CPolygon2D* poly, float sx, float sy)
{
	// this function scalesthe local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);


	CMatrix3x2 ms; // used to hold scaling transform matrix

	// initialize the matrix with translation values dx dy
	ms.Mat_Init_3X2(&ms, sx, 0,
		0, sy,
		0, 0);


	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// scale and store result back

		// create a 1x2 matrix to do the transform
		CMatrix1x2 p0 = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y };
		CMatrix1x2 p1 = { 0, 0 }; // this will hold result

		// now scale via a matrix multiply
		Mat_Mul_1X2_3X2(&p0, &ms, &p1);

		// now copy the result back into vertex
		poly->vlist[curr_vert].x = p1.M[0];
		poly->vlist[curr_vert].y = p1.M[1];

	} // end for curr_vert
	/*	// used to hold scaling transform matrix
	// initialize the matrix with translation values dx dy
	double lfTemp[] = { sx, 0,
	0, sy,
	0, 0 };
	CMatrix oMatrix3x2(3, 2, lfTemp);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
	// scale and store result back

	// create a 1x2 matrix to do the transform
	double lf1x2[] = { poly->vlist[curr_vert].x, poly->vlist[curr_vert].y, 0 };
	CMatrix oMatix1x2(1, 3, lf1x2);
	// now scale via a matrix multiply

	// now copy the result back into vertex
	CMatrix oRes = oMatix1x2*oMatrix3x2;

	// now copy the result back into vertex
	poly->vlist[curr_vert].x = oRes.m_pMatrix[0][0];
	poly->vlist[curr_vert].y = oRes.m_pMatrix[0][1];

	} // end for curr_vert*/
	// return success
	return(1);

} // end Scale_Polygon2D_Mat

////////////////////////////////////////////////////////////////

int CMath2::Mat_Mul_1X3_3X3(CMatrix1x3* ma,
	CMatrix3x3* mb,
	CMatrix1x3* mprod)
{
	// this function multiplies a 1x3 matrix against a 
	// 3x3 matrix - ma*mb and stores the result

	for (int col = 0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb

		float sum = 0; // used to hold result

		for (int index = 0; index < 3; index++)
		{
			// add in next product pair
			sum += (ma->M[index] * mb->M[index][col]);
		} // end for index

		// insert resulting col element
		mprod->M[col] = sum;

	} // end for col

	return(1);

} // end Mat_Mul_1X3_3X3

////////////////////////////////////////////////////////////////

int CMath2::Mat_Mul_1X2_3X2(CMatrix1x2* ma,
	CMatrix3x2* mb,
	CMatrix1x2* mprod)
{
	// this function multiplies a 1x2 matrix against a 
	// 3x2 matrix - ma*mb and stores the result
	// using a dummy element for the 3rd element of the 1x2 
	// to make the matrix multiply valid i.e. 1x3 X 3x2

	for (int col = 0; col < 2; col++)
	{
		// compute dot product from row of ma 
		// and column of mb

		float sum = 0; // used to hold result

		int index = 0;
		for (; index < 2; index++)
		{
			// add in next product pair
			sum += (ma->M[index] * mb->M[index][col]);
		} // end for index

		// add in last element * 1 
		sum += mb->M[index][col];

		// insert resulting col element
		mprod->M[col] = sum;

	} // end for col

	return(1);

} // end Mat_Mul_1X2_3X2

//////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

int CMath2::Scale_Polygon2D(CPolygon2D* poly, float sx, float sy)
{
	// this function scalesthe local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{
		// scale and store result back
		poly->vlist[curr_vert].x *= sx;
		poly->vlist[curr_vert].y *= sy;

	} // end for curr_vert

	// return success
	return(1);

} // end Scale_Polygon2D

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

// the following 3 functions are the standard transforms (no matrices)

int CMath2::Translate_Polygon2D(CPolygon2D* poly, int dx, int dy)
{
	// this function translates the center of a polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// translate
	poly->x0 += dx;
	poly->y0 += dy;

	// return success
	return(1);

} // end Translate_Polygon2D


int CMath2::Rotate_Polygon2D(CPolygon2D* poly, int theta)
{
	// this function rotates the local coordinates of the polygon

	// test for valid pointer
	if (!poly)
		return(0);

	// test for negative rotation angle
	if (theta < 0)
		theta += 360;

	// loop and rotate each point, very crude, no lookup!!!
	for (int curr_vert = 0; curr_vert < poly->num_verts; curr_vert++)
	{

		// perform rotation
		double xr = (double)poly->vlist[curr_vert].x*CMathBase::cos_look[theta] -
			(double)poly->vlist[curr_vert].y*CMathBase::sin_look[theta];

		double yr = (double)poly->vlist[curr_vert].x*CMathBase::sin_look[theta] +
			(double)poly->vlist[curr_vert].y*CMathBase::cos_look[theta];

		// store result back
		poly->vlist[curr_vert].x = xr;
		poly->vlist[curr_vert].y = yr;

	} // end for curr_vert

	// return success
	return(1);

} // end Rotate_Polygon2D

////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

// FUNCTIONS //////////////////////////////////////////////

FIXP16 FIXP16_MUL(FIXP16 fp1, FIXP16 fp2)
{
	// this function computes the product fp_prod = fp1*fp2
	// using 64 bit math, so as not to loose precission

	FIXP16 fp_prod; // return the product

	_asm {
		mov eax, fp1      // move into eax fp2
			imul fp2          // multiply fp1*fp2
			shrd eax, edx, 16 // result is in 32:32 format 
			// residing at edx:eax
			// shift it into eax alone 16:16
			// result is sitting in eax
	} // end asm

} // end FIXP16_MUL

///////////////////////////////////////////////////////////////

FIXP16 FIXP16_DIV(FIXP16 fp1, FIXP16 fp2)
{
	// this function computes the quotient fp1/fp2 using
	// 64 bit math, so as not to loose precision

	_asm {
		mov eax, fp1      // move dividend into eax
			cdq               // sign extend it to edx:eax
			shld edx, eax, 16 // now shift 16:16 into position in edx
			sal eax, 16       // and shift eax into position since the
			// shld didn't move it -- DUMB! uPC
			idiv fp2          // do the divide
			// result is sitting in eax     
	} // end asm

} // end FIXP16_DIV

///////////////////////////////////////////////////////////////

void FIXP16_Print(FIXP16 fp)
{
	// this function prints out a fixed point number
	Write_Error("\nfp=%f", (float)(fp) / FIXP16_MAG);

} // end FIXP16_Print

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

float CMath2::Fast_Sin(float theta)
{
	return CMathBase::Fast_Sin(theta);
} // end Fast_Sin

///////////////////////////////////////////////////////////////

float CMath2::Fast_Cos(float theta)
{
	return CMathBase::Fast_Cos(theta);
} // end Fast_Cos

void SoaringLoong::Math::CMath2::Build_Sin_Cos_Tables(void)
{
	CMathBase::BuildFastTable();
}

///////////////////////////////////////////////////////////////

void POLAR2D_To_POINT2D(POLAR2D_PTR polar, POINT2D_PTR rect)
{
	// convert polar to rectangular
	rect->x = polar->r*cosf(polar->theta);
	rect->y = polar->r*sinf(polar->theta);

} // end POLAR2D_To_POINT2D

////////////////////////////////////////////////////////////////

void POLAR2D_To_RectXY(POLAR2D_PTR polar, float *x, float *y)
{
	// convert polar to rectangular
	*x = polar->r*cosf(polar->theta);
	*y = polar->r*sinf(polar->theta);

} // end POLAR2D_To_RectXY

///////////////////////////////////////////////////////////////

void POINT2D_To_POLAR2D(POINT2D_PTR rect, POLAR2D_PTR polar)
{
	// convert rectangular to polar
	polar->r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	polar->theta = atanf(rect->y / rect->x);

} // end POINT2D_To_POLAR2D

////////////////////////////////////////////////////////////

void POINT2D_To_PolarRTh(POINT2D_PTR rect, float *r, float *theta)
{
	// convert rectangular to polar
	*r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	*theta = atanf(rect->y / rect->x);

} // end POINT2D_To_PolarRTh

///////////////////////////////////////////////////////////////

void CYLINDRICAL3D_To_POINT3D(CYLINDRICAL3D_PTR cyl, POINT3D_PTR rect)
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

void SPHERICAL3D_To_POINT3D(SPHERICAL3D_PTR sph, POINT3D_PTR rect)
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

void SPHERICAL3D_To_RectXYZ(SPHERICAL3D_PTR sph,
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

void POINT3D_To_SPHERICAL3D(POINT3D_PTR rect, SPHERICAL3D_PTR sph)
{
	// convert rectangular to spherical
	sph->p = sqrtf((rect->x*rect->x) + (rect->y*rect->y) + (rect->z*rect->z));

	sph->theta = atanf(rect->y / rect->x);

	// we need r to compute phi
	float r = sph->p*sinf(sph->phi);

	sph->phi = asinf(r / sph->p);

} // end POINT3D_To_CYLINDRICAL3D

////////////////////////////////////////////////////////////

void POINT3D_To_SphericalPThPh(POINT3D_PTR rect,
	float *p, float *theta, float *phi)
{
	// convert rectangular to spherical
	*p = sqrtf((rect->x*rect->x) + (rect->y*rect->y) + (rect->z*rect->z));
	*theta = atanf(rect->y / rect->x);

	// we need r to compute phi
	float r = sqrtf((rect->x * rect->x) + (rect->y * rect->y));
	*phi = asinf(r / (*p));

} // end POINT3D_To_SphericalPThPh

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

// these are the 4D version of the vector functions, they
// assume that the vectors are 3D with a w, so w is left
// out of all the operations



///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
