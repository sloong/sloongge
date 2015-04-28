// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <ddraw.h>    // directX includes
//#include <DxErr.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","rstEOF") rename("BOF","rstBOF")

#include <sys/timeb.h>
#include <time.h>
#include <string>
using std::string;
using std::wstring;
#ifdef _UNICODE
typedef wstring tstring;
typedef const wstring ctstring;
#else
typedef string tstring;
typedef const string ctstring;
#endif // _UNICODE

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <exception>
using std::exception;

#include <list>
using std::list;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		{if(NULL != (p)){delete (p);(p)=NULL;}}
#endif	// SAFE_DELETE


#ifndef HR
#define HR(x)    { hr = x; if( FAILED(hr) ) { return hr; } }         //自定义一个HR宏，方便执行错误的返回
#endif

#ifndef SAFE_DELETE					
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }       //自定义一个SAFE_RELEASE()宏,便于指针资源的释放
#endif    

#ifndef SAFE_RELEASE			
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }     //自定义一个SAFE_RELEASE()宏,便于COM资源的释放
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// bit manipulation macros
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
// used for swapping algorithm
#define SWAP(a,b,t) {t=a; a=b; b=t;}

// used to compute the min and max of two expresions
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 
#define MAX(a, b)  (((a) > (b)) ? (b) : (a)) 
// convert integer and float to fixed point 16.16
#define INT_TO_FIXP16(i) ((i) <<  FIXP16_SHIFT)
#define FLOAT_TO_FIXP16(f) (((float)(f) * (float)FIXP16_MAG+0.5))

// convert fixed point to float
#define FIXP16_TO_FLOAT(fp) ( ((float)fp)/FIXP16_MAG)

// extract the whole part and decimal part from a fixed point 16.16
#define FIXP16_WP(fp) ((fp) >> FIXP16_SHIFT)
#define FIXP16_DP(fp) ((fp) && FIXP16_DP_MASK)

// macros to clear out matrices
#define MAT_ZERO_2X2(m) {memset((void *)(m), 0, sizeof(MATRIX2X2));}
#define MAT_ZERO_3X3(m) {memset((void *)(m), 0, sizeof(MATRIX3X3));}
#define MAT_ZERO_4X4(m) {memset((void *)(m), 0, sizeof(MATRIX4X4));}
#define MAT_ZERO_4X3(m) {memset((void *)(m), 0, sizeof(MATRIX4X3));}

// macros to set the identity matrix
#define MAT_IDENTITY_2X2(m) {memcpy((void *)(m), (void *)&IMAT_2X2, sizeof(MATRIX2X2));}
#define MAT_IDENTITY_3X3(m) {memcpy((void *)(m), (void *)&IMAT_3X3, sizeof(MATRIX3X3));}
#define MAT_IDENTITY_4X4(m) {memcpy((void *)(m), (void *)&IMAT_4X4, sizeof(MATRIX4X4));}
#define MAT_IDENTITY_4X3(m) {memcpy((void *)(m), (void *)&IMAT_4X3, sizeof(MATRIX4X3));}
// pi defines
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 

// DEFINES & CONSTANTS /////////////////////////////////////

// defines for small numbers
#define EPSILON_E3 (float)(1E-3) 
#define EPSILON_E4 (float)(1E-4) 
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)

// some math macros
#define DEG_TO_RAD(ang) ((ang)*PI/180.0)
#define RAD_TO_DEG(rads) ((rads)*180.0/PI)

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))


// UNICODE define
#ifdef UNICODE
#define stscanf swscanf
#define stscanf_s swscanf_s
#define fgetts fgetws
#else
#define stscanf sscanf
#define stscanf_s swscanf_s
#define fgetts fgets
#endif
