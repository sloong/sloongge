#include "stdafx.h"
#include "SloongMathBase.h"
using namespace SoaringLoong::Math;

float SoaringLoong::Math::CMathBase::sin_look[361] = {0.0f};

float SoaringLoong::Math::CMathBase::cos_look[361] = {0.0f};

bool SoaringLoong::Math::CMathBase::isBuildFastTable = false;


float CMathBase::Fast_Distance_3D(float fx, float fy, float fz)
{
	// this function computes the distance from the origin to x,y,z

	int temp;  // used for swaping
	int x, y, z; // used for algorithm

	// make sure values are all positive
	x = fabs(fx) * 1024;
	y = fabs(fy) * 1024;
	z = fabs(fz) * 1024;

	// sort values
	if (y < x) SWAP(x, y, temp)

	if (z < y) SWAP(y, z, temp)

	if (y < x) SWAP(x, y, temp)

		int dist = (z + 11 * (y >> 5) + (x >> 2));

	// compute distance with 8% error
	return((float)(dist >> 10));

} // end Fast_Distance_3D

void SoaringLoong::Math::CMathBase::BuildFastTable()
{
	// create sin/cos lookup table
	// note the creation of one extra element; 360
	// this helps with logic in using the tables
	if ( isBuildFastTable )
	{
		return;
	}

	// generate the tables 0 - 360 inclusive
	for (int ang = 0; ang <= 360; ang++)
	{
		// convert ang to radians
		float theta = (float)ang*PI / (float)180;

		// insert next entry into table
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);

	} // end for ang
	isBuildFastTable = true;
}

float SoaringLoong::Math::CMathBase::Fast_Sin(float theta)
{
	// this function uses the sin_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// sin computation that the blind lookup, but with with
	// a slight hit in speed
	if ( isBuildFastTable == false )
	{
		BuildFastTable();
	}
	// convert angle to 0-359
	theta = fmodf(theta, 360);

	// make angle positive
	if (theta < 0) theta += 360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(sin_look[theta_int] +
		theta_frac*(sin_look[theta_int + 1] - sin_look[theta_int]));

}

float SoaringLoong::Math::CMathBase::Fast_Cos(float theta)
{
	// this function uses the cos_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// cos computation that the blind lookup, but with with
	// a slight hit in speed
	if (isBuildFastTable == false)
	{
		BuildFastTable();
	}
	// convert angle to 0-359
	theta = fmodf(theta, 360);

	// make angle positive
	if (theta < 0) theta += 360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(cos_look[theta_int] +
		theta_frac*(cos_look[theta_int + 1] - cos_look[theta_int]));
}

SoaringLoong::Math::CMathBase::CMathBase()
{
	BuildFastTable();
}

int FIXP16_MUL(int fp1, int fp2)
{
	// this function computes the product fp_prod = fp1*fp2
	// using 64 bit math, so as not to loose precission

	int fp_prod; // return the product

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

int FIXP16_DIV(int fp1, int fp2)
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
