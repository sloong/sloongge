#include "stdafx.h"
#include "SloongVector.h"
#include "SloongMath2.h"
using namespace SoaringLoong::Math::Vector;



void CVector2D::VECTOR2D_Add(VECTOR2D_PTR va,
	VECTOR2D_PTR vb,
	VECTOR2D_PTR vsum)
{
	// this function adds va+vb and return it in vsum
	vsum->x = va->x + vb->x;
	vsum->y = va->y + vb->y;

} // end VECTOR2D_Add

////////////////////////////////////////////////////////////

VECTOR2D CVector2D::VECTOR2D_Add(VECTOR2D_PTR va,
	VECTOR2D_PTR vb)
{
	// this function adds va+vb and returns the result on 
	// the stack
	VECTOR2D vsum;

	vsum.x = va->x + vb->x;
	vsum.y = va->y + vb->y;

	// return result
	return(vsum);

} // end VECTOR2D_Add

////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Sub(VECTOR2D_PTR va,
	VECTOR2D_PTR vb,
	VECTOR2D_PTR vdiff)
{
	// this function subtracts va-vb and return it in vdiff
	// the stack
	vdiff->x = va->x - vb->x;
	vdiff->y = va->y - vb->y;

} // end VECTOR2D_Sub

////////////////////////////////////////////////////////////

VECTOR2D CVector2D::VECTOR2D_Sub(VECTOR2D_PTR va,
	VECTOR2D_PTR vb)
{
	// this function subtracts va-vb and returns the result on 
	// the stack
	VECTOR2D vdiff;

	vdiff.x = va->x - vb->x;
	vdiff.y = va->y - vb->y;

	// return result
	return(vdiff);

} // end VECTOR2D_Sub

////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Scale(float k,
	VECTOR2D_PTR va,
	VECTOR2D_PTR vscaled)
{
	// this function scales a vector by the constant k,
	// leaves the original unchanged, and returns the result
	// in vscaled

	// multiply each component by scaling factor
	vscaled->x = k*va->x;
	vscaled->y = k*va->y;

} // end VECTOR2D_Scale

/////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Scale(float k, VECTOR2D_PTR va)
{
	// this function scales a vector by the constant k,
	// by scaling the original

	// multiply each component by scaling factor
	va->x *= k;
	va->y *= k;

} // end VECTOR2D_Scale

//////////////////////////////////////////////////////////////

float CVector2D::VECTOR2D_Dot(VECTOR2D_PTR va, VECTOR2D_PTR vb)
{
	// computes the dot product between va and vb
	return((va->x * vb->x) + (va->y * vb->y));
} // end VECTOR2D_Dot

///////////////////////////////////////////////////////////////

float CVector2D::VECTOR2D_Length(VECTOR2D_PTR va)
{
	// computes the magnitude of a vector, slow

	return(sqrtf(va->x*va->x + va->y*va->y));

} // end VECTOR2D_Length

///////////////////////////////////////////////////////////////

float CVector2D::VECTOR2D_Length_Fast(VECTOR2D_PTR va)
{
	// computes the magnitude of a vector using an approximation
	// very fast
	return((float)Fast_Distance_2D(va->x, va->y));

} // end VECTOR2D_Length_Fast

///////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Normalize(VECTOR2D_PTR va)
{
	// normalizes the sent vector in place

	// compute length
	float length = sqrtf(va->x*va->x + va->y*va->y);

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1 / length;

	// compute normalized version of vector
	va->x = va->x*length_inv;
	va->y = va->y*length_inv;

} // end VECTOR2D_Normalize

///////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Normalize(VECTOR2D_PTR va, VECTOR2D_PTR vn)
{
	// normalizes the sent vector and returns the result in vn

	VECTOR2D_ZERO(vn);

	// compute length
	float length = (float)sqrtf(va->x*va->x + va->y*va->y);

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1 / length;

	// compute normalized version of vector
	vn->x = va->x*length_inv;
	vn->y = va->y*length_inv;

} // end VECTOR2D_Normalize

///////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Build(VECTOR2D_PTR init,
	VECTOR2D_PTR term,
	VECTOR2D_PTR result)
{
	// this function creates a vector from two vectors (or points)
	//  in 3D space

	result->x = term->x - init->x;
	result->y = term->y - init->y;

} // end VECTOR2D_Build

///////////////////////////////////////////////////////////////

float CVector2D::VECTOR2D_CosTh(VECTOR2D_PTR va, VECTOR2D_PTR vb)
{
	// this function returns the cosine of the angle between
	// two vectors. Note, we could compute the actual angle,
	// many many times, in further calcs we will want ultimately
	// compute cos of the angle, so why not just leave it!
	return(VECTOR2D_Dot(va, vb) / (VECTOR2D_Length(va)*VECTOR2D_Length(vb)));

} // end VECTOR2D_CosTh

//////////////////////////////////////////////////////////////

void CVector2D::VECTOR2D_Print(VECTOR2D_PTR va, char *name = "v")
{
	// this function prints out a VECTOR2D

	Write_Error("\n%s=[", name);
	for (int index = 0; index < 2; index++)
		Write_Error("%f, ", va->M[index]);
	Write_Error("]");

} // end VECTOR2D_Print


int CVector2D::Fast_Distance_2D(int x, int y)
{
	// this function computes the distance from 0,0 to x,y with 3.5% error

	// first compute the absolute value of x,y
	x = abs(x);
	y = abs(y);

	// compute the minimum of x,y
	int mn = MIN(x, y);

	// return the distance
	return(x + y - (mn >> 1) - (mn >> 2) + (mn >> 4));

} // end Fast_Distance_2D



void CVector3D::VECTOR3D_Add(VECTOR3D_PTR va,
	VECTOR3D_PTR vb,
	VECTOR3D_PTR vsum)
{
	// this function adds va+vb and return it in vsum
	vsum->x = va->x + vb->x;
	vsum->y = va->y + vb->y;
	vsum->z = va->z + vb->z;

} // end VECTOR3D_Add

////////////////////////////////////////////////////////////

VECTOR3D CVector3D::VECTOR3D_Add(VECTOR3D_PTR va,
	VECTOR3D_PTR vb)
{
	// this function adds va+vb and returns the result on 
	// the stack
	VECTOR3D vsum;

	vsum.x = va->x + vb->x;
	vsum.y = va->y + vb->y;
	vsum.z = va->z + vb->z;

	// return result
	return(vsum);

} // end VECTOR3D_Add

////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Sub(VECTOR3D_PTR va,
	VECTOR3D_PTR vb,
	VECTOR3D_PTR vdiff)
{
	// this function subtracts va-vb and return it in vdiff
	// the stack
	vdiff->x = va->x - vb->x;
	vdiff->y = va->y - vb->y;
	vdiff->z = va->z - vb->z;

} // end VECTOR3D_Sub

////////////////////////////////////////////////////////////

VECTOR3D CVector3D::VECTOR3D_Sub(VECTOR3D_PTR va, VECTOR3D_PTR vb)
{
	// this function subtracts va-vb and returns the result on 
	// the stack
	VECTOR3D vdiff;

	vdiff.x = va->x - vb->x;
	vdiff.y = va->y - vb->y;
	vdiff.z = va->z - vb->z;

	// return result
	return(vdiff);

} // end VECTOR3D_Sub

////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Scale(float k, VECTOR3D_PTR va)
{
	// this function scales a vector by the constant k,
	// and modifies the original

	// multiply each component by scaling factor
	va->x *= k;
	va->y *= k;
	va->z *= k;

} // end VECTOR3D_Scale

/////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Scale(float k, VECTOR3D_PTR va, VECTOR3D_PTR vscaled)
{
	// this function scales a vector by the constant k,
	// leaves the original unchanged, and returns the result
	// in vres as well as on the stack

	// multiply each component by scaling factor
	vscaled->x = k*va->x;
	vscaled->y = k*va->y;
	vscaled->z = k*va->z;

} // end VECTOR3D_Scale

//////////////////////////////////////////////////////////////

float CVector3D::VECTOR3D_Dot(VECTOR3D_PTR va, VECTOR3D_PTR vb)
{
	// computes the dot product between va and vb
	return((va->x * vb->x) + (va->y * vb->y) + (va->z * vb->z));
} // end VECTOR3D_DOT
/////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vn)
{
	// this function computes the cross product between va and vb
	// and returns the vector that is perpendicular to each in vn

	vn->x = ((va->y * vb->z) - (va->z * vb->y));
	vn->y = -((va->x * vb->z) - (va->z * vb->x));
	vn->z = ((va->x * vb->y) - (va->y * vb->x));

} // end VECTOR3D_Cross

/////////////////////////////////////////////////////////////

VECTOR3D CVector3D::VECTOR3D_Cross(VECTOR3D_PTR va, VECTOR3D_PTR vb)
{
	// this function computes the cross product between va and vb
	// and returns the vector that is perpendicular to each

	VECTOR3D vn;

	vn.x = ((va->y * vb->z) - (va->z * vb->y));
	vn.y = -((va->x * vb->z) - (va->z * vb->x));
	vn.z = ((va->x * vb->y) - (va->y * vb->x));

	// return result
	return(vn);

} // end VECTOR3D_Cross

//////////////////////////////////////////////////////////////

float CVector3D::Length() const
{
	// computes the magnitude of a vector, slow

	return((float)sqrtf(x*x + y*y + z*z));

} // end VECTOR3D_Length

///////////////////////////////////////////////////////////////

float CVector3D::VECTOR3D_Length_Fast(VECTOR3D_PTR va)
{
	// computes the magnitude of a vector using an approximation
	// very fast
	return(Fast_Distance_3D(va->x, va->y, va->z));

} // end VECTOR3D_Length_Fast

///////////////////////////////////////////////////////////////

void CVector3D::Normalize()
{
	// normalizes the sent vector in placew

	// compute length
	float length = sqrtf(x*x + y*y + z*z);

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1 / length;

	// compute normalized version of vector
	x *= length_inv;
	y *= length_inv;
	z *= length_inv;

} // end VECTOR3D_Normalize

///////////////////////////////////////////////////////////////

void CVector3D::Normalize(const CVector3D& va)
{
	// normalizes the sent vector and returns the result in vn

	this->Zero();

	// compute length
	float length = va.Length();

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1.0 / length;

	// compute normalized version of vector
	this->x = va.x*length_inv;
	this->y = va.y*length_inv;
	this->z = va.z*length_inv;

} // end VECTOR3D_Normalize

///////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Build(VECTOR3D_PTR init, VECTOR3D_PTR term, VECTOR3D_PTR result)
{
	// this function creates a vector from two vectors (or points)
	//  in 3D space
	result->x = term->x - init->x;
	result->y = term->y - init->y;
	result->z = term->z - init->z;

} // end VECTOR3D_Build

/////////////////////////////////////////////////////////////

float CVector3D::VECTOR3D_CosTh(VECTOR3D_PTR va, VECTOR3D_PTR vb)
{
	// this function returns the cosine of the angle between
	// two vectors. Note, we could compute the actual angle,
	// many many times, in further calcs we will want ultimately
	// compute cos of the angle, so why not just leave it!
	return(VECTOR3D_Dot(va, vb) / (va->Length()*vb->Length()));

} // end VECTOR3D_CosTh

///////////////////////////////////////////////////////////////

void CVector3D::VECTOR3D_Print(VECTOR3D_PTR va, char *name = "v")
{
	// this function prints out a VECTOR3D

	Write_Error("\n%s=[", name);
	for (int index = 0; index < 3; index++)
		Write_Error("%f, ", va->M[index]);
	Write_Error("]");

} // end VECTOR3D_Print

////////////////////////////////////////////////////////////

CVector4D CVector4D::Add(const CVector4D& va, const CVector4D& vb)
{
	return Add(&va, &vb);
} // end VECTOR4D_Add

////////////////////////////////////////////////////////////

CVector4D CVector4D::Subtract(const CVector4D& va, const CVector4D& vb)
{
	return Subtract(&va, &vb);
} // end VECTOR4D_Sub

////////////////////////////////////////////////////////////

void CVector4D::Subtract(const CVector4D& va)
{
	// this function subtracts va-vb and returns the result on 
	// the stack
	x -= va.x;
	y -= va.y;
	z -= va.z;
	w = 1;
} // end VECTOR4D_Sub

////////////////////////////////////////////////////////////

void CVector4D::VECTOR4D_Scale(float k, VECTOR4D_PTR va)
{
	// this function scales a vector by the constant k,
	// in place , note w is left unchanged

	// multiply each component by scaling factor
	va->x *= k;
	va->y *= k;
	va->z *= k;
	va->w = 1;

} // end VECTOR4D_Scale

/////////////////////////////////////////////////////////////

void CVector4D::VECTOR4D_Scale(float k, VECTOR4D_PTR va, VECTOR4D_PTR vscaled)
{
	// this function scales a vector by the constant k,
	// leaves the original unchanged, and returns the result
	// in vres as well as on the stack

	// multiply each component by scaling factor
	vscaled->x = k*va->x;
	vscaled->y = k*va->y;
	vscaled->z = k*va->z;
	vscaled->w = 1;

} // end VECTOR4D_Scale

//////////////////////////////////////////////////////////////

float CVector4D::Dot(const CVector4D& va, const CVector4D& vb)
{
	// computes the dot product between va and vb
	return((va.x * vb.x) + (va.y * vb.y) + (va.z * vb.z));
} // end VECTOR4D_DOT

/////////////////////////////////////////////////////////////

VECTOR4D CVector4D::Cross(const CVector4D& va, const CVector4D& vb)
{
	// this function computes the cross product between va and vb
	// and returns the vector that is perpendicular to each

	VECTOR4D vn;

	vn.x = ((va.y * vb.z) - (va.z * vb.y));
	vn.y = -((va.x * vb.z) - (va.z * vb.x));
	vn.z = ((va.x * vb.y) - (va.y * vb.x));
	vn.w = 1;

	// return result
	return(vn);

} // end VECTOR4D_Cross

//////////////////////////////////////////////////////////////

float CVector4D::Length()
{
	// computes the magnitude of a vector, slow

	return(sqrtf(this->x*this->x + this->y*this->y + this->z*this->z));

} // end VECTOR4D_Length

///////////////////////////////////////////////////////////////

float CVector4D::Length_Fast()
{
	// computes the magnitude of a vector using an approximation
	// very fast
	return(Fast_Distance_3D(this->x, this->y, this->z));

} // end VECTOR4D_Length_Fast

///////////////////////////////////////////////////////////////

void CVector4D::Normalize()
{
	// normalizes the sent vector and returns the result
	
	// compute length
	float length = Length();

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1.0 / length;

	// compute normalized version of vector
	this->x *= length_inv;
	this->y *= length_inv;
	this->z *= length_inv;
	this->w = 1;

} // end VECTOR4D_Normalize

///////////////////////////////////////////////////////////////

void CVector4D::Normalize(VECTOR4D_PTR va)
{
	// normalizes the sent vector and returns the result in vn

	Zero();

	// compute length
	float length = Length();

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5)
		return;

	float length_inv = 1.0 / length;

	// compute normalized version of vector
	this->x = va->x*length_inv;
	this->y = va->y*length_inv;
	this->z = va->z*length_inv;
	this->w = 1;

} // end VECTOR4D_Normalize

///////////////////////////////////////////////////////////////

float CVector4D::VECTOR4D_CosTh(VECTOR4D_PTR va, VECTOR4D_PTR vb)
{
	// this function returns the cosine of the angle between
	// two vectors. Note, we could compute the actual angle,
	// many many times, in further calcs we will want ultimately
	// compute cos of the angle, so why not just leave it!
	return(CVector4D::Dot(*va, *vb) / (va->Length()*vb->Length()));

} // end VECTOR4D_CosTh

////////////////////////////////////////////////////////////

void CVector4D::VECTOR4D_Print(VECTOR4D_PTR va, char *name = "v")
{
	// this function prints out a VECTOR4D

	Write_Error("\n%s[", name);
	for (int index = 0; index < 4; index++)
		Write_Error("%f, ", va->M[index]);
	Write_Error("]");

} // end VECTOR4D_Print

SoaringLoong::Math::Vector::CVector4D::CVector4D(int x, int y, int z, int w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

SoaringLoong::Math::Vector::CVector4D::CVector4D(int x, int y, int z)
{
	Initialize(x, y, z);
}

SoaringLoong::Math::Vector::CVector4D::CVector4D(const CVector4D& vSrc)
{
	Copy(vSrc);
}

void SoaringLoong::Math::Vector::CVector4D::Add(const CVector4D& va)
{
	Copy(Add(this, &va));
}

void SoaringLoong::Math::Vector::CVector4D::Add(const CVector4D* va)
{
	Copy( Add(this, va));
}

CVector4D CVector4D::Add( const CVector4D* va, const CVector4D* vb)
{
	// this function adds va+vb and returns the result on 
	// the stack
	VECTOR4D vsum;

	vsum.x = va->x + vb->x;
	vsum.y = va->y + vb->y;
	vsum.z = va->z + vb->z;
	vsum.w = 1;

	// return result
	return(vsum);
}

CVector4D SoaringLoong::Math::Vector::CVector4D::Multiply(const CVector4D& vector, const CMatrix4x4& matrix)
{
	return Multiply(&vector, &matrix);
}

void SoaringLoong::Math::Vector::CVector4D::Multiply(const CMatrix4x4& matrix)
{
	*this = Multiply(*this, matrix);
}

CVector4D SoaringLoong::Math::Vector::CVector4D::Multiply(const CVector4D* vector, const CMatrix4x4* matrix)
{
	// this function multiplies a VECTOR4D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function makes no assumptions

	CVector4D pTemp;
	for (int col = 0; col < 4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row = 0; row < 4; row++)
		{
			// add in next product pair
			sum += (vector->M[row] * matrix->M[row][col]);
		} // end for index

		// insert resulting col element
		pTemp.M[col] = sum;

	} // end for col
	return pTemp;
}

float SoaringLoong::Math::Vector::CVector4D::Dot(const CVector4D& va)
{
	return Dot(*this, va);
}

CVector4D SoaringLoong::Math::Vector::CVector4D::Subtract(const CVector4D* va, const CVector4D* vb)
{
	// this function subtracts va-vb and return it in vdiff
	// the stack
	CVector4D vTemp;
	vTemp.x = va->x - vb->x;
	vTemp.y = va->y - vb->y;
	vTemp.z = va->z - vb->z;
	vTemp.w = 1;
	return vTemp;
}

void SoaringLoong::Math::Vector::CVector4D::Build(const CVector4D& vInit, const CVector4D& vTerm)
{
	Build(&vInit, &vTerm);
}

void SoaringLoong::Math::Vector::CVector4D::Build(const CVector4D* vInit, const CVector4D* vTerm)
{
	x = vTerm->x - vInit->x;
	y = vTerm->y - vInit->y;
	z = vTerm->z - vInit->z;
	w = 1;
}
