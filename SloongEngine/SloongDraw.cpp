#include "stdafx.h"
#include "SloongDraw.h"
#include "IUniversal.h"
#include "SlMath.h"
#include "SloongMath2.h"

#pragma comment(lib,"Universal.lib")
#pragma comment(lib,"SLMath.lib")

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

#define WIN32_LEAN_AND_MEAN  

CDDraw::CDDraw()
{
	m_nWidth = 1024;            // width of screen
	m_nHeight = 768;           // height of screen
	m_nBPP = 32;              // bits per pixel
	m_bIsWindowed = 0;                       // is this a windowed app?    
	m_rcWindows.left = 0;                             // clipping rectangle 
	m_rcWindows.right = (m_nWidth - 1);
	m_rcWindows.top = 0;
	m_rcWindows.bottom = (m_nHeight - 1);

	start_clock_count = 0;     // used for timing



	// notice that interface 7.0 is used on a number of interfaces
	m_pDDraw = NULL;  // dd object
	m_pPrimarySurface = NULL;  // dd primary surface
	m_pBackSurface = NULL;  // dd back surface
//	lpddpal = NULL;  // a pointer to the created dd palette
	m_pWindowClipper = NULL; // dd clipper for window

	m_stSurfaceDesc;                 // a direct draw surface description struct
	m_pPrimaryBuffer = NULL; // primary video buffer
	m_pBackBuffer = NULL; // secondary back buffer
	m_dwPrimaryPitch = 0;    // memory line pitch for primary buffer
	m_dwBackPitch = 0;    // memory line pitch for back buffer

}


CDDraw::~CDDraw()
{
}

//////////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER CDDraw::DDrawAttachClipper(LPDIRECTDRAW7 lpDDraw, LPDIRECTDRAWSURFACE7 lpDDrawSurface, int nNumRect, LPRECT lpRectList)
{
	LPDIRECTDRAWCLIPPER lpDDrawClipper = NULL;
	LPRGNDATA lpRgnData = NULL;

	if (FAILED(lpDDraw->CreateClipper(0, &lpDDrawClipper, NULL)))
	{
		return NULL;
	}

	lpRgnData = (LPRGNDATA)new BYTE[sizeof(lpRgnData->rdh) + sizeof(RECT)*nNumRect];

	memcpy(lpRgnData->Buffer, lpRectList, sizeof(RECT)*nNumRect);

	lpRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	lpRgnData->rdh.iType = RDH_RECTANGLES;
	lpRgnData->rdh.nCount = nNumRect;
	lpRgnData->rdh.nRgnSize = nNumRect*sizeof(RECT);
	lpRgnData->rdh.rcBound.bottom = 0;
	lpRgnData->rdh.rcBound.top = 0;
	lpRgnData->rdh.rcBound.left = 0;
	lpRgnData->rdh.rcBound.right = 0;

	for (int i = 0; i < nNumRect; i++)
	{
		if (lpRectList[i].top <= lpRgnData->rdh.rcBound.top)
		{
			lpRgnData->rdh.rcBound.top = lpRectList[i].top;
		}

		if (lpRectList[i].bottom >= lpRgnData->rdh.rcBound.bottom)
		{
			lpRgnData->rdh.rcBound.bottom = lpRectList[i].bottom;
		}

		if (lpRectList[i].left <= lpRgnData->rdh.rcBound.left)
		{
			lpRgnData->rdh.rcBound.left = lpRectList[i].left;
		}

		if (lpRectList[i].right <= lpRgnData->rdh.rcBound.right)
		{
			lpRgnData->rdh.rcBound.right = lpRectList[i].right;
		}
	}

	if (FAILED(lpDDrawClipper->SetClipList(lpRgnData, 0)))
	{
		delete lpRgnData;
		return NULL;
	}

	if (FAILED(lpDDrawSurface->SetClipper(lpDDrawClipper)))
	{
		delete lpRgnData;
		return NULL;
	}

	delete lpRgnData;
	return lpDDrawClipper;
}


LPDIRECTDRAWCLIPPER CDDraw::DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
	int num_rects,
	LPRECT clip_list)

{
	// this function creates a clipper from the sent clip list and attaches
	// it to the sent surface

	int index;                         // looping var
	LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
	LPRGNDATA region_data;             // pointer to the region data that contains
	// the header and clip list

	// first create the direct draw clipper
	if (FAILED(m_pDDraw->CreateClipper(0, &lpddclipper, NULL)))
		return(NULL);

	// now create the clip list from the sent data

	// first allocate memory for region data
	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

	// now copy the rects into region data
	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	// set up fields of header
	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = num_rects*sizeof(RECT);

	region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.top = 64000;
	region_data->rdh.rcBound.right = -64000;
	region_data->rdh.rcBound.bottom = -64000;

	// find bounds of all clipping regions
	for (index = 0; index < num_rects; index++)
	{
		// test if the next rectangle unioned with the current bound is larger
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;

	} // end for index

	// now we have computed the bounding rectangle region and set up the data
	// now let's set the clipping list

	if (FAILED(lpddclipper->SetClipList(region_data, 0)))
	{
		// release memory and return error
		free(region_data);
		return(NULL);
	} // end if

	// now attach the clipper to the surface
	if (FAILED(lpdds->SetClipper(lpddclipper)))
	{
		// release memory and return error
		free(region_data);
		return(NULL);
	} // end if

	// all is well, so release memory and send back the pointer to the new clipper
	free(region_data);
	return(lpddclipper);

} // end DDraw_Attach_Clipper




int CDDraw::DDrawFillSurface(LPDIRECTDRAWSURFACE7 lpDDrawSurface, COLORREF nColor, CRect* rcClient /* = NULL */)
{
	DDBLTFX ddbltfx; // this contains the DDBLTFX structure

	// clear out the structure and set the size field 
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the dwfillcolor field to the desired color
	ddbltfx.dwFillColor = nColor;

	// ready to blt to surface
	lpDDrawSurface->Blt(rcClient,       // ptr to dest rectangle
		NULL,       // ptr to source surface, NA            
		NULL,       // ptr to source rectangle, NA
		DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
		&ddbltfx);  // ptr to DDBLTFX structure

	// return success
	return(1);
} // end DDraw_Fill_Surface


int CDDraw::Draw_Pixel(int x, int y, COLORREF color, LPBYTE video_buffer, int lpitch)
{
	// this function plots a single pixel at x,y with color
	if (x >= m_nWidth || y >= m_nHeight)
	{
		return 0;
	}

	LPCOLORREF pBuffer = (LPCOLORREF)video_buffer;

	if (pBuffer)
	{
		pBuffer[x + y*lpitch] = color;
	}



	// return success
	return(1);

} // end Draw_Pixel


//  Parameters:
//			x0,y0:
//				starting position
//			x1,y1:
//				ending position
//			color:
//				line color
//			vb_start:
//				point to video buffer 
//			lpitch:
//				memory pitch
int CDDraw::Draw_Line(int x0, int y0, int x1, int y1, COLORREF color, LPBYTE vb_start, int lpitch)
{
	// this function draws a line from xo,yo to x1,y1 using differential error
	// terms (based on Bresenahams work)

	int dx,             // difference in x's
		dy,             // difference in y's
		dx2,            // dx,dy * 2
		dy2,
		x_inc,          // amount in pixel space to move during drawing
		y_inc,          // amount in pixel space to move during drawing
		error,          // the discriminant i.e. error i.e. decision variable
		index;          // used for looping

	LPBYTE pBuffer = (LPBYTE)vb_start;
	// pre-compute first pixel address in video buffer
	pBuffer = pBuffer + x0*sizeof(COLORREF)+y0*lpitch;

	// compute horizontal and vertical deltas
	dx = x1 - x0;
	dy = y1 - y0;

	// test which direction the line is going in i.e. slope angle
	if (dx >= 0)
	{
		x_inc = sizeof(COLORREF);

	} // end if line is moving right
	else
	{
		x_inc = sizeof(COLORREF);
		x_inc *= -1;
		dx = -dx;  // need absolute value

	} // end else moving left

	// test y component of slope

	if (dy >= 0)
	{
		y_inc = lpitch;
	} // end if line is moving down
	else
	{
		y_inc = -lpitch;
		dy = -dy;  // need absolute value

	} // end else moving up

	// compute (dx,dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	// now based on which delta is greater we can draw the line
	if (dx > dy)
	{
		// initialize error term
		error = dy2 - dx;

		// draw the line
		for (index = 0; index <= dx; index++)
		{
			// set the pixel
			COLORREF* pTarget = (COLORREF*)pBuffer;
			*pTarget = color;

			// test if error has overflowed
			if (error >= 0)
			{
				error -= dx2;

				// move to next line
				pBuffer += y_inc;

			} // end if error overflowed

			// adjust the error term
			error += dy2;

			// move to the next pixel
			pBuffer += x_inc;

		} // end for

	} // end if |slope| <= 1
	else
	{
		// initialize error term
		error = dx2 - dy;

		// draw the line
		for (index = 0; index <= dy; index++)
		{
			// set the pixel
			COLORREF* pTarget = (COLORREF*)pBuffer;
			*pTarget = color;

			// test if error overflowed
			if (error >= 0)
			{
				error -= dy2;

				// move to next line
				pBuffer += x_inc;

			} // end if error overflowed

			// adjust the error term
			error += dx2;

			// move to the next pixel
			pBuffer += y_inc;

		} // end for

	} // end else |slope| > 1

	// return success
	return(1);

} // end Draw_Line




///////////////////////////////////////////////////////////////////////

int CDDraw::DrawPolygon2D(CPolygon2D* poly, LPBYTE vbuffer, int lpitch, RECT rcScreen)
{
	// this function draws a POLYGON2D based on 

	// test if the polygon is visible
	if (poly->state)
	{
		// loop thru and draw a line from vertices 1 to n
		int index = 0;
		for (index = 0; index < poly->num_verts - 1; index++)
		{
			// draw line from ith to ith+1 vertex
			DrawClipLine((int)poly->vlist[index].x + poly->x0,
				(int)poly->vlist[index].y + poly->y0,
				(int)poly->vlist[index + 1].x + poly->x0,
				(int)poly->vlist[index + 1].y + poly->y0,
				poly->color,
				vbuffer, lpitch, rcScreen);

		} // end for

		// now close up polygon
		// draw line from last vertex to 0th
		DrawClipLine((int)poly->vlist[0].x + poly->x0,
			(int)poly->vlist[0].y + poly->y0,
			(int)poly->vlist[index].x + poly->x0,
			(int)poly->vlist[index].y + poly->y0,
			poly->color,
			vbuffer, lpitch, rcScreen);

		// return success
		return(1);
	} // end if
	else
		return(0);

} // end Draw_Polygon2D


///////////////////////////////////////////////////////////////////////

// these are the matrix versions, note they are more inefficient for
// single transforms, but their power comes into play when you concatenate
// multiple transformations, not to mention that all transforms are accomplished
// with the same code, just the matrix differs
///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int CDDraw::DrawText(LPCTSTR strText, int x, int y, COLORREF dwColor, LPDIRECTDRAWSURFACE7 lpdds)
{
	// this function draws the sent text on the sent surface 
	// using color index as the color in the palette

	HDC xdc; // the working dc

	// get the dc from surface
	if (FAILED(lpdds->GetDC(&xdc)))
		return(0);

	// set the colors for the text up
	SetTextColor(xdc, dwColor);

	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOut(xdc, x, y, strText, _tcslen(strText));

	// release the dc
	lpdds->ReleaseDC(xdc);

	// return success
	return(1);
} // end Draw_Text_GDI

///////////////////////////////////////////////////////////////

HRESULT CDDraw::DrawClipLine(int x0, int y0, int x1, int y1, COLORREF color)
{
	return DrawClipLine(x0, y0, x1, y1, color, GetBackBuffer(), GetBackPitch(), m_rcWindows);
}


HRESULT CDDraw::DrawClipLine(int x0, int y0, int x1, int y1, COLORREF color, LPBYTE lpVideoBuffer, int lpitch, RECT rcScreen)
{
	// this helper function draws a clipped line

	int cxs, cys,
		cxe, cye;

	// clip and draw each line
	cxs = x0;
	cys = y0;
	cxe = x1;
	cye = y1;

	// clip the line
	if (Clip_Line(cxs, cys, cxe, cye, rcScreen))
		Draw_Line(cxs, cys, cxe, cye, color, (LPBYTE)lpVideoBuffer, lpitch);

	// return success
	return(1);

} // end Draw_Clip_Line

///////////////////////////////////////////////////////////

int CDDraw::Clip_Line(int &x1, int &y1, int &x2, int &y2, RECT rcScreen)
{
	// this function clips the sent line using the globally defined clipping
	// region

	// internal clipping codes
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

	int xc1 = x1,
		yc1 = y1,
		xc2 = x2,
		yc2 = y2;

	int p1_code = 0,
		p2_code = 0;

	CRect rcTemp;
	rcTemp.left = rcScreen.left;
	rcTemp.top = rcScreen.top;
	rcTemp.right = rcScreen.right - 1;
	rcTemp.bottom = rcScreen.bottom - 1;
	// determine codes for p1 and p2
	if (y1 < rcTemp.top)
		p1_code |= CLIP_CODE_N;
	else
	if (y1 > rcTemp.bottom)
		p1_code |= CLIP_CODE_S;

	if (x1 < rcTemp.left)
		p1_code |= CLIP_CODE_W;
	else
	if (x1 > rcTemp.right)
		p1_code |= CLIP_CODE_E;

	if (y2 < rcTemp.top)
		p2_code |= CLIP_CODE_N;
	else
	if (y2 > rcTemp.bottom)
		p2_code |= CLIP_CODE_S;

	if (x2 < rcTemp.left)
		p2_code |= CLIP_CODE_W;
	else
	if (x2 > rcTemp.right)
		p2_code |= CLIP_CODE_E;

	// try and trivially reject
	if ((p1_code & p2_code))
		return(0);

	// test for totally visible, if so leave points untouched
	if (p1_code == 0 && p2_code == 0)
		return(1);

	// determine end clip point for p1
	switch (p1_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
						yc1 = rcTemp.top;
						xc1 = (int)(x1 + 0.5 + (rcTemp.top - y1)*(x2 - x1) / (y2 - y1));
	} break;
	case CLIP_CODE_S:
	{
						yc1 = rcTemp.bottom;
						xc1 = (int)(x1 + 0.5 + (rcTemp.bottom - y1)*(x2 - x1) / (y2 - y1));
	} break;

	case CLIP_CODE_W:
	{
						xc1 = rcTemp.left;
						yc1 = (int)(y1 + 0.5 + (rcTemp.left - x1)*(y2 - y1) / (x2 - x1));
	} break;

	case CLIP_CODE_E:
	{
						xc1 = rcTemp.right;
						yc1 = (int)(y1 + 0.5 + (rcTemp.right - x1)*(y2 - y1) / (x2 - x1));
	} break;

	// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
	{
						 // north hline intersection
						 yc1 = rcTemp.top;
						 xc1 = (int)(x1 + 0.5 + (rcTemp.top - y1)*(x2 - x1) / (y2 - y1));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc1 < rcTemp.left || xc1 > rcTemp.right)
						 {
							 // east vline intersection
							 xc1 = rcTemp.right;
							 yc1 = (int)(y1 + 0.5 + (rcTemp.right - x1)*(y2 - y1) / (x2 - x1));
						 } // end if

	} break;

	case CLIP_CODE_SE:
	{
						 // south hline intersection
						 yc1 = rcTemp.bottom;
						 xc1 = (int)(x1 + 0.5 + (rcTemp.bottom - y1)*(x2 - x1) / (y2 - y1));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc1 < rcTemp.left || xc1 > rcTemp.right)
						 {
							 // east vline intersection
							 xc1 = rcTemp.right;
							 yc1 = (int)(y1 + 0.5 + (rcTemp.right - x1)*(y2 - y1) / (x2 - x1));
						 } // end if

	} break;

	case CLIP_CODE_NW:
	{
						 // north hline intersection
						 yc1 = rcTemp.top;
						 xc1 = (int)(x1 + 0.5 + (rcTemp.top - y1)*(x2 - x1) / (y2 - y1));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc1 < rcTemp.left || xc1 > rcTemp.right)
						 {
							 xc1 = rcTemp.left;
							 yc1 = (int)(y1 + 0.5 + (rcTemp.left - x1)*(y2 - y1) / (x2 - x1));
						 } // end if

	} break;

	case CLIP_CODE_SW:
	{
						 // south hline intersection
						 yc1 = rcTemp.bottom;
						 xc1 = (int)(x1 + 0.5 + (rcTemp.bottom - y1)*(x2 - x1) / (y2 - y1));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc1 < rcTemp.left || xc1 > rcTemp.right)
						 {
							 xc1 = rcTemp.left;
							 yc1 = (int)(y1 + 0.5 + (rcTemp.left - x1)*(y2 - y1) / (x2 - x1));
						 } // end if

	} break;

	default:break;

	} // end switch

	// determine clip point for p2
	switch (p2_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
						yc2 = rcTemp.top;
						xc2 = x2 + (rcTemp.top - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_S:
	{
						yc2 = rcTemp.bottom;
						xc2 = x2 + (rcTemp.bottom - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_W:
	{
						xc2 = rcTemp.left;
						yc2 = y2 + (rcTemp.left - x2)*(y1 - y2) / (x1 - x2);
	} break;

	case CLIP_CODE_E:
	{
						xc2 = rcTemp.right;
						yc2 = y2 + (rcTemp.right - x2)*(y1 - y2) / (x1 - x2);
	} break;

	// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
	{
						 // north hline intersection
						 yc2 = rcTemp.top;
						 xc2 = (int)(x2 + 0.5 + (rcTemp.top - y2)*(x1 - x2) / (y1 - y2));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc2 < rcTemp.left || xc2 > rcTemp.right)
						 {
							 // east vline intersection
							 xc2 = rcTemp.right;
							 yc2 = (int)(y2 + 0.5 + (rcTemp.right - x2)*(y1 - y2) / (x1 - x2));
						 } // end if

	} break;

	case CLIP_CODE_SE:
	{
						 // south hline intersection
						 yc2 = rcTemp.bottom;
						 xc2 = (int)(x2 + 0.5 + (rcTemp.bottom - y2)*(x1 - x2) / (y1 - y2));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc2 < rcTemp.left || xc2 > rcTemp.right)
						 {
							 // east vline intersection
							 xc2 = rcTemp.right;
							 yc2 = (int)(y2 + 0.5 + (rcTemp.right - x2)*(y1 - y2) / (x1 - x2));
						 } // end if

	} break;

	case CLIP_CODE_NW:
	{
						 // north hline intersection
						 yc2 = rcTemp.top;
						 xc2 = (int)(x2 + 0.5 + (rcTemp.top - y2)*(x1 - x2) / (y1 - y2));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc2 < rcTemp.left || xc2 > rcTemp.right)
						 {
							 xc2 = rcTemp.left;
							 yc2 = (int)(y2 + 0.5 + (rcTemp.left - x2)*(y1 - y2) / (x1 - x2));
						 } // end if

	} break;

	case CLIP_CODE_SW:
	{
						 // south hline intersection
						 yc2 = rcTemp.bottom;
						 xc2 = (int)(x2 + 0.5 + (rcTemp.bottom - y2)*(x1 - x2) / (y1 - y2));

						 // test if intersection is valid, of so then done, else compute next
						 if (xc2 < rcTemp.left || xc2 > rcTemp.right)
						 {
							 xc2 = rcTemp.left;
							 yc2 = (int)(y2 + 0.5 + (rcTemp.left - x2)*(y1 - y2) / (x1 - x2));
						 } // end if

	} break;

	default:break;

	} // end switch

	// do bounds check
	if ((xc1 < rcTemp.left) || (xc1 > rcTemp.right) ||
		(yc1 < rcTemp.top) || (yc1 > rcTemp.bottom) ||
		(xc2 < rcTemp.left) || (xc2 > rcTemp.right) ||
		(yc2 < rcTemp.top) || (yc2 > rcTemp.bottom))
	{
		return(0);
	} // end if

	// store vars back
	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return(1);

} // end Clip_Line

int CDDraw::DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, // source surface to draw
	int x, int y,                 // position to draw at
	int width, int height,        // size of source surface
	LPDIRECTDRAWSURFACE7 dest,    // surface to draw the surface on
	int transparent /*= 1*/)          // transparency flag
{
	//g_oLog.Log( Format(TEXT("Draw surface  %d,%d in %d,%d "),x,y,width,height));
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             

	// fill in the destination rect
	dest_rect.left = x;
	dest_rect.top = y;
	dest_rect.right = x + width - 1;
	dest_rect.bottom = y + height - 1;

	// fill in the source rect
	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = width - 1;
	source_rect.bottom = height - 1;

	// test transparency flag

	if (transparent)
	{
		// enable color key blit
		// blt to destination surface
		if (FAILED(dest->Blt(&dest_rect, source, &source_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
			return(0);

	} // end if
	else
	{
		// perform blit without color key
		// blt to destination surface
		if (FAILED(dest->Blt(&dest_rect, source,
			&source_rect, (DDBLT_WAIT),
			NULL)))
			return(0);

	} // end if

	// return success
	return(1);

} // end DDraw_Draw_Surface
void CDDraw::PlotPixel(int x, int y, COLORREF dwColor, LPVOID pVideoBuffer, int nLpitch)
{
	// This function plots a pixel in 24-bit color mode 
	// assuming that the caller already locked the surface 
	// and is sending a pointer and byte pitch to it.

	// First build up color WORD
	DWORD* pBuffer = (DWORD*)pVideoBuffer;
	pBuffer += y*(nLpitch / sizeof(DWORD));
	pBuffer += x;

	// Write the data.
	*pBuffer = dwColor;

}	// end PlotPixel32


inline void CDDraw::Mem_Set_WORD(void *dest, USHORT data, int count)
{
	// this function fills or sets unsigned 16-bit aligned memory
	// count is number of words
#ifndef _WIN64
	_asm
	{
		mov edi, dest; edi points to destination memory
			mov ecx, count; number of 16 - bit words to move
			mov ax, data; 16 - bit data
			rep stosw; move data
	} // end asm
#endif // !_WIN64

} // end Mem_Set_WORD

///////////////////////////////////////////////////////////

inline void CDDraw::Mem_Set_QUAD(void *dest, UINT data, int count)
{
	// this function fills or sets unsigned 32-bit aligned memory
	// count is number of quads
#ifndef _WIN64
	_asm
	{
		mov edi, dest; edi points to destination memory
			mov ecx, count; number of 32 - bit words to move
			mov eax, data; 32 - bit data
			rep stosd; move data
	} // end asm
#endif
} // end Mem_Set_QUAD

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

int CDDraw::Initialize(HWND hWnd, int width, int height, int bpp, bool bFullScreen)
{
	// this function initializes directdraw
	int index; // looping variable

	m_hWnd = hWnd;
	// create IDirectDraw interface 7.0 object and test for error
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&m_pDDraw, IID_IDirectDraw7, NULL)))
		return(0);

	// based on windowed or fullscreen set coorperation level
	if (bFullScreen)
	{
		// set cooperation level to fullscreen mode 
		if (FAILED(m_pDDraw->SetCooperativeLevel(hWnd,
			DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN |
			DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED)))
			return(0);

		// set the display mode
		if (FAILED(m_pDDraw->SetDisplayMode(width, height, bpp, 0, 0)))
			return(0);
		
	} // end if
	else
	{
		// set cooperation level to windowed mode 
		if (FAILED(m_pDDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
			return(0);
	} // end else

	// set globals
	m_nHeight = height;
	m_nWidth = width;
	m_nBPP = bpp;
	m_bIsWindowed = !bFullScreen;

	// Create the primary surface
	memset(&m_stSurfaceDesc, 0, sizeof(m_stSurfaceDesc));
	m_stSurfaceDesc.dwSize = sizeof(m_stSurfaceDesc);

	// we need to let dd know that we want a complex 
	// flippable surface structure, set flags for that
	if (bFullScreen)
	{
		// fullscreen mode
		m_stSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		m_stSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

		// set the backbuffer count to 0 for windowed mode
		// 1 for fullscreen mode, 2 for triple buffering
		m_stSurfaceDesc.dwBackBufferCount = 1;
	} // end if
	else
	{
		// windowed mode
		m_stSurfaceDesc.dwFlags = DDSD_CAPS;
		m_stSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// set the backbuffer count to 0 for windowed mode
		// 1 for fullscreen mode, 2 for triple buffering
		m_stSurfaceDesc.dwBackBufferCount = 0;
	} // end else

	// create the primary surface
	m_pDDraw->CreateSurface(&m_stSurfaceDesc, &m_pPrimarySurface, NULL);

	// get the pixel format of the primary surface
	DDPIXELFORMAT ddpf; // used to get pixel format

	// initialize structure
	DDRAW_INIT_STRUCT(ddpf);

	// query the format from primary surface
	m_pPrimarySurface->GetPixelFormat(&ddpf);


	// only need a backbuffer for fullscreen modes
	if (!m_bIsWindowed)
	{
		DDSCAPS2 ddscaps;
		// query for the backbuffer i.e the secondary surface
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (FAILED(m_pPrimarySurface->GetAttachedSurface(&ddscaps, &m_pBackSurface)))
			return(0);

	} // end if
	else
	{
		// must be windowed, so create a double buffer that will be blitted
		// rather than flipped as in full screen mode
		m_pBackSurface = CreateSurface(width, height); // int mem_flags, USHORT color_key_flag);

	} // end else


	// clear out both primary and secondary surfaces
	if (m_bIsWindowed)
	{
		// only clear backbuffer
		DDrawFillSurface(m_pBackSurface, 0);
	} // end if
	else
	{
		// fullscreen, simply clear everything
		DDrawFillSurface(m_pPrimarySurface, 0);
		DDrawFillSurface(m_pBackSurface, 0);
	} // end else

	// set software algorithmic clipping region
	m_rcWindows.left = 0;
	m_rcWindows.right = m_nWidth - 1;
	m_rcWindows.top = 0;
	m_rcWindows.bottom = m_nHeight - 1;

	// setup backbuffer clipper always
	RECT screen_rect = { 0, 0, m_nWidth, m_nHeight };
	// »ñÈ¡Clipper 
	//DDraw_Attach_Clipper(m_pBackSurface, 1, &screen_rect);

	// set up windowed mode clipper
	if (m_bIsWindowed)
	{
		// set windowed clipper
		if (FAILED(m_pDDraw->CreateClipper(0, &m_pWindowClipper, NULL)))
			return(0);

		if (FAILED(m_pWindowClipper->SetHWnd(0, hWnd)))
			return(0);

		if (FAILED(m_pPrimarySurface->SetClipper(m_pWindowClipper)))
			return(0);
	} // end if screen windowed

	// return success
	return(1);

} // end DDraw_Init

///////////////////////////////////////////////////////////

int CDDraw::DDraw_Shutdown(void)
{
	// this function release all the resources directdraw
	// allocated, mainly to com objects

	// release the clippers first

	if (m_pWindowClipper)
		m_pWindowClipper->Release();

	// release the palette if there is one
//	if (lpddpal)
//		lpddpal->Release();

	// release the secondary surface
	if (m_pBackSurface)
		m_pBackSurface->Release();

	// release the primary surface
	if (m_pPrimarySurface)
		m_pPrimarySurface->Release();

	// finally, the main dd object
	if (m_pDDraw)
		m_pDDraw->Release();

	// return success
	return(1);
} // end DDraw_Shutdown

///////////////////////////////////////////////////////////   
LPDIRECTDRAWSURFACE7 CDDraw::CreateSurface(int width,int height,int mem_flags,	COLORREF color_key_value)
{
	// this function creates an offscreen plain surface

	DDSURFACEDESC2 ddsd;         // working description
	LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface

	// set to access caps, width, and height
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	
	// set dimensions of the new bitmap surface
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	// set surface to offscreen plain
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	// create the surface
	if (FAILED(m_pDDraw->CreateSurface(&ddsd, &lpdds, NULL)))
		return(NULL);

	// set color key to default color 000
	// note that if this is a 8bit bob then palette index 0 will be 
	// transparent by default
	// note that if this is a 16bit bob then RGB value 000 will be 
	// transparent
	if (color_key_value >= 0)
	{
		DDCOLORKEY color_key; // used to set color key
		color_key.dwColorSpaceLowValue = color_key_value;
		color_key.dwColorSpaceHighValue = color_key_value;
		// now set the color key for source blitting
		lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	}
	

	// return surface
	return(lpdds);
} // end DDraw_Create_Surface

///////////////////////////////////////////////////////////   

int CDDraw::DDraw_Flip(void)
{
	// this function flip the primary surface with the secondary surface

	// test if either of the buffers are locked
	if (m_pPrimaryBuffer || m_pBackBuffer)
		return(0);

	// flip pages
	if (!m_bIsWindowed)
	{
		while (FAILED(m_pPrimarySurface->Flip(NULL, DDFLIP_WAIT)));
	}
	else
	{
		RECT    dest_rect;    // used to compute destination rectangle
		GetWindowRect(m_hWnd, &dest_rect);

		CRect rcWindow, rcClient;
		// get the window's rectangle in screen coordinates
		GetWindowRect(m_hWnd, &rcWindow);
		GetClientRect(m_hWnd, &rcClient);

		// compute the destination rectangle
		dest_rect.left += m_rcWindows.left + rcWindow.Width()-rcClient.Width();
		dest_rect.top += m_rcWindows.top + rcWindow.Height() - rcClient.Height();

		dest_rect.right = dest_rect.left + m_nWidth;
		dest_rect.bottom = dest_rect.top + m_nHeight;
		// blit the entire back surface to the primary
		if (FAILED(m_pPrimarySurface->Blt(&dest_rect, m_pBackSurface, NULL, DDBLT_WAIT, NULL)))
			return(0);

	} // end if

	// return success
	return(1);

} // end DDraw_Flip

///////////////////////////////////////////////////////////   

int CDDraw::DDraw_Wait_For_Vsync(void)
{
	// this function waits for a vertical blank to begin

	m_pDDraw->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);

	// return success
	return(1);
} // end DDraw_Wait_For_Vsync

///////////////////////////////////////////////////////////   

UCHAR* CDDraw::DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch)
{
	// this function locks the sent surface and returns a pointer to it

	// is this surface valid
	if (!lpdds)
		return(NULL);

	// lock the surface
	DDRAW_INIT_STRUCT(m_stSurfaceDesc);
	lpdds->Lock(NULL, &m_stSurfaceDesc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// set the memory pitch
	if (lpitch)
		*lpitch = m_stSurfaceDesc.lPitch;

	// return pointer to surface
	return((UCHAR *)m_stSurfaceDesc.lpSurface);

} // end DDraw_Lock_Surface

///////////////////////////////////////////////////////////   

int CDDraw::DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds)
{
	// this unlocks a general surface

	// is this surface valid
	if (!lpdds)
		return(0);

	// unlock the surface memory
	lpdds->Unlock(NULL);

	// return success
	return(1);
} // end DDraw_Unlock_Surface

///////////////////////////////////////////////////////////

UCHAR* CDDraw::DDraw_Lock_Primary_Surface(void)
{
	// this function locks the priamary surface and returns a pointer to it
	// and updates the global variables primary_buffer, and primary_lpitch

	// is this surface already locked
	if (m_pPrimaryBuffer)
	{
		// return to current lock
		return(m_pPrimaryBuffer);
	} // end if

	// lock the primary surface
	DDRAW_INIT_STRUCT(m_stSurfaceDesc);
	m_pPrimarySurface->Lock(NULL, &m_stSurfaceDesc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// set globals
	m_pPrimaryBuffer = (UCHAR *)m_stSurfaceDesc.lpSurface;
	m_dwPrimaryPitch = m_stSurfaceDesc.lPitch;

	// return pointer to surface
	return(m_pPrimaryBuffer);

} // end DDraw_Lock_Primary_Surface

///////////////////////////////////////////////////////////   

HRESULT CDDraw::DDraw_Unlock_Primary_Surface(void)
{
	// this unlocks the primary

	// is this surface valid
	if (!m_pPrimaryBuffer)
		return S_FALSE;

	// unlock the primary surface
	m_pPrimarySurface->Unlock(NULL);

	// reset the primary surface
	m_pPrimaryBuffer = NULL;
	m_dwPrimaryPitch = 0;

	// return success
	return S_OK;
} // end DDraw_Unlock_Primary_Surface

//////////////////////////////////////////////////////////

UCHAR* CDDraw::DDraw_Lock_Back_Surface(void)
{
	// this function locks the secondary back surface and returns a pointer to it
	// and updates the global variables secondary buffer, and back_lpitch

	// is this surface already locked
	if (m_pBackBuffer)
	{
		// return to current lock
		return(m_pBackBuffer);
	} // end if

	// lock the primary surface
	DDRAW_INIT_STRUCT(m_stSurfaceDesc);
	m_pBackSurface->Lock(NULL, &m_stSurfaceDesc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// set globals
	m_pBackBuffer = (UCHAR *)m_stSurfaceDesc.lpSurface;
	m_dwBackPitch = m_stSurfaceDesc.lPitch;

	// return pointer to surface
	return(m_pBackBuffer);

} // end DDraw_Lock_Back_Surface

///////////////////////////////////////////////////////////   

HRESULT CDDraw::DDraw_Unlock_Back_Surface(void)
{
	// this unlocks the secondary

	// is this surface valid
	if (!m_pBackBuffer)
		return S_FALSE;

	// unlock the secondary surface
	HRESULT hRes = m_pBackSurface->Unlock(NULL);

	// reset the secondary surface
	m_pBackBuffer = NULL;
	m_dwBackPitch = 0;

	// return success
	return hRes;
} // end DDraw_Unlock_Back_Surface

///////////////////////////////////////////////////////////

DWORD CDDraw::Get_Clock(void)
{
	// this function returns the current tick count

	// return time
	return(GetTickCount());

} // end Get_Clock

///////////////////////////////////////////////////////////

DWORD CDDraw::Start_Clock(void)
{
	// this function starts the clock, that is, saves the current
	// count, use in conjunction with Wait_Clock()

	return(start_clock_count = Get_Clock());

} // end Start_Clock

////////////////////////////////////////////////////////////

DWORD CDDraw::Wait_Clock(DWORD count)
{
	// this function is used to wait for a specific number of clicks
	// since the call to Start_Clock

	while ((Get_Clock() - start_clock_count) < count);
	return(Get_Clock());

} // end Wait_Clock


int CDDraw::Draw_Rectangle(int x1, int y1, int x2, int y2, int color,
	LPDIRECTDRAWSURFACE7 lpdds)
{
	// this function uses directdraw to draw a filled rectangle

	DDBLTFX ddbltfx; // this contains the DDBLTFX structure
	RECT fill_area;  // this contains the destination rectangle

	// clear out the structure and set the size field 
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the dwfillcolor field to the desired color
	ddbltfx.dwFillColor = color;

	// fill in the destination rectangle data (your data)
	fill_area.top = y1;
	fill_area.left = x1;
	fill_area.bottom = y2;
	fill_area.right = x2;

	// ready to blt to surface, in this case blt to primary
	lpdds->Blt(&fill_area, // ptr to dest rectangle
		NULL,       // ptr to source surface, NA            
		NULL,       // ptr to source rectangle, NA
		DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
		&ddbltfx);  // ptr to DDBLTFX structure

	// return success
	return(1);

} // end Draw_Rectangle

///////////////////////////////////////////////////////////

int CDDraw::Rotate_Colors(int start_index, int end_index)
{
	// this function rotates the color between start and end

	int colors = end_index - start_index + 1;

	PALETTEENTRY work_pal[MAX_COLORS_PALETTE]; // working palette

	// get the color palette
// 	lpddpal->GetEntries(0, start_index, colors, work_pal);
// 
// 	// shift the colors
// 	lpddpal->SetEntries(0, start_index + 1, colors - 1, work_pal);
// 
// 	// fix up the last color
// 	lpddpal->SetEntries(0, start_index, 1, &work_pal[colors - 1]);

	// return success
	return(1);

} // end Rotate_Colors

///////////////////////////////////////////////////////////   

int CDDraw::Blink_Colors(int command, BLINKER_PTR new_light, int id)
{
	// this function blinks a set of lights

	static BLINKER lights[256]; // supports up to 256 blinking lights
	static int initialized = 0; // tracks if function has initialized

	// test if this is the first time function has ran
	if (!initialized)
	{
		// set initialized
		initialized = 1;

		// clear out all structures
		memset((void *)lights, 0, sizeof(lights));

	} // end if

	// now test what command user is sending
	switch (command)
	{
	case BLINKER_ADD: // add a light to the database
	{
						  // run thru database and find an open light
						  for (int index = 0; index < 256; index++)
						  {
							  // is this light available?
							  if (lights[index].state == 0)
							  {
								  // set light up
								  lights[index] = *new_light;

								  // set internal fields up
								  lights[index].counter = 0;
								  lights[index].state = -1; // off

								  // update palette entry
								  //lpddpal->SetEntries(0, lights[index].color_index, 1, &lights[index].off_color);

								  // return id to caller
								  return(index);

							  } // end if

						  } // end for index

	} break;

	case BLINKER_DELETE: // delete the light indicated by id
	{
							 // delete the light sent in id 
							 if (lights[id].state != 0)
							 {
								 // kill the light
								 memset((void *)&lights[id], 0, sizeof(BLINKER));

								 // return id
								 return(id);

							 } // end if
							 else
								 return(-1); // problem


	} break;

	case BLINKER_UPDATE: // update the light indicated by id
	{
							 // make sure light is active
							 if (lights[id].state != 0)
							 {
								 // update on/off parms only
								 lights[id].on_color = new_light->on_color;
								 lights[id].off_color = new_light->off_color;
								 lights[id].on_time = new_light->on_time;
								 lights[id].off_time = new_light->off_time;

								 // update palette entry
								 //if (lights[id].state == -1)
									 //lpddpal->SetEntries(0, lights[id].color_index, 1, &lights[id].off_color);
								// else
									 //lpddpal->SetEntries(0, lights[id].color_index, 1, &lights[id].on_color);

								 // return id
								 return(id);

							 } // end if
							 else
								 return(-1); // problem

	} break;

	case BLINKER_RUN: // run the algorithm
	{
						  // run thru database and process each light
						  for (int index = 0; index < 256; index++)
						  {
							  // is this active?
							  if (lights[index].state == -1)
							  {
								  // update counter
								  if (++lights[index].counter >= lights[index].off_time)
								  {
									  // reset counter
									  lights[index].counter = 0;

									  // change states 
									  lights[index].state = -lights[index].state;

									  // update color
									  //lpddpal->SetEntries(0, lights[index].color_index, 1, &lights[index].on_color);

								  } // end if

							  } // end if
							  else
							  if (lights[index].state == 1)
							  {
								  // update counter
								  if (++lights[index].counter >= lights[index].on_time)
								  {
									  // reset counter
									  lights[index].counter = 0;

									  // change states 
									  lights[index].state = -lights[index].state;

									  // update color
									  //lpddpal->SetEntries(0, lights[index].color_index, 1, &lights[index].off_color);

								  } // end if
							  } // end else if

						  } // end for index

	} break;

	default: break;

	} // end switch

	// return success
	return(1);

} // end Blink_Colors

//////////////////////////////////////////////////////////////////////////////


void CDDraw::HLine(int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch)
{
	// draw a horizontal line using the memset function

	int temp; // used for temporary storage during endpoint swap

	// perform trivial rejections
	if (y > m_rcWindows.bottom || y < m_rcWindows.top)
		return;

	// sort x1 and x2, so that x2 > x1
	if (x1>x2)
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
	} // end swap

	// perform trivial rejections
	if (x1 > m_rcWindows.right || x2 < m_rcWindows.left)
		return;

	// now clip
	x1 = ((x1 < m_rcWindows.left) ? m_rcWindows.left : x1);
	x2 = ((x2 > m_rcWindows.right) ? m_rcWindows.right : x2);

	// draw the row of pixels
	memset((UCHAR *)(vbuffer + (y*lpitch) + x1),
		(UCHAR)color, x2 - x1 + 1);

} // end HLine

//////////////////////////////////////////////////////////////////////////////

void CDDraw::VLine(int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch)
{
	// draw a vertical line, note that a memset function can no longer be
	// used since the pixel addresses are no longer contiguous in memory
	// note that the end points of the line must be on the screen

	UCHAR *start_offset; // starting memory offset of line

	int index, // loop index
		temp;  // used for temporary storage during swap


	// perform trivial rejections
	if (x > m_rcWindows.right || x < m_rcWindows.left)
		return;

	// make sure y2 > y1
	if (y1>y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;
	} // end swap

	// perform trivial rejections
	if (y1 > m_rcWindows.bottom || y2 < m_rcWindows.top)
		return;

	// now clip
	y1 = ((y1 < m_rcWindows.top) ? m_rcWindows.top : y1);
	y2 = ((y2 > m_rcWindows.bottom) ? m_rcWindows.bottom : y2);

	// compute starting position
	start_offset = vbuffer + (y1*lpitch) + x;

	// draw line one pixel at a time
	for (index = 0; index <= y2 - y1; index++)
	{
		// set the pixel
		*start_offset = (UCHAR)color;

		// move downward to next line
		start_offset += lpitch;

	} // end for index

} // end VLine

///////////////////////////////////////////////////////////

void CDDraw::Screen_Transitions(int effect, UCHAR *vbuffer, int lpitch)
{
	// this function can be called to perform a myraid of screen transitions
	// to the destination buffer, make sure to save and restore the palette
	// when performing color transitions in 8-bit modes

	int pal_reg;         // used as loop counter
	int index;           // used as loop counter
	int red, green, blue;           // used in fad algorithm

	PALETTEENTRY color;              // temporary color
	PALETTEENTRY work_palette[MAX_COLORS_PALETTE];  // used as a working palette
	PALETTEENTRY work_color;         // used in color algorithms

	// test which screen effect is being selected
	switch (effect)
	{
	case SCREEN_DARKNESS:
	{
							// fade to black

							for (index = 0; index < 80; index++)
							{
								// process each color
								for (pal_reg = 1; pal_reg<MAX_COLORS_PALETTE; pal_reg++)
								{
									// get the entry data
									color = work_palette[pal_reg];

									// test if this color register is already black
									if (color.peRed > 4) color.peRed -= 3;
									else
										color.peRed = 0;

									if (color.peGreen > 4) color.peGreen -= 3;
									else
										color.peGreen = 0;

									if (color.peBlue > 4) color.peBlue -= 3;
									else
										color.peBlue = 0;

									// set the color to a diminished intensity
									work_palette[pal_reg] = color;

								} // end for pal_reg

								// wait a bit

								Start_Clock(); Wait_Clock(12);

							} // end for index

	} break;

	case SCREEN_WHITENESS:
	{
							 // fade to white
							 for (index = 0; index < 64; index++)
							 {
								 // loop thru all palette registers
								 for (pal_reg = 0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
								 {
									 // get the entry data
									 color = work_palette[pal_reg];

									 // make 32 bit copy of color
									 red = color.peRed;
									 green = color.peGreen;
									 blue = color.peBlue;

									 if ((red += 4) >= 255)
										 red = 255;

									 if ((green += 4) >= 255)
										 green = 255;

									 if ((blue += 4) >= 255)
										 blue = 255;

									 // store colors back
									 color.peRed = red;
									 color.peGreen = green;
									 color.peBlue = blue;

									 // set the color to a diminished intensity
									 work_palette[pal_reg] = color;

								 } // end for pal_reg


								 // wait a bit

								 Start_Clock(); Wait_Clock(12);

							 } // end for index

	} break;

	case SCREEN_REDNESS:
	{
						   // fade to red

						   for (index = 0; index < 64; index++)
						   {

							   // loop thru all palette registers
							   for (pal_reg = 0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
							   {
								   // get the entry data
								   color = work_palette[pal_reg];

								   // make 32 bit copy of color
								   red = color.peRed;
								   green = color.peGreen;
								   blue = color.peBlue;

								   if ((red += 6) >= 255)
									   red = 255;

								   if ((green -= 4) < 0)
									   green = 0;

								   if ((blue -= 4) < 0)
									   blue = 0;

								   // store colors back
								   color.peRed = red;
								   color.peGreen = green;
								   color.peBlue = blue;

								   // set the color to a diminished intensity
								   work_palette[pal_reg] = color;

							   } // end for pal_reg



							   // wait a bit

							   Start_Clock(); Wait_Clock(12);

						   } // end for index

	} break;

	case SCREEN_BLUENESS:
	{
							// fade to blue

							for (index = 0; index < 64; index++)
							{

								// loop thru all palette registers
								for (pal_reg = 0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
								{
									// get the entry data
									color = work_palette[pal_reg];

									// make 32 bit copy of color
									red = color.peRed;
									green = color.peGreen;
									blue = color.peBlue;

									if ((red -= 4) < 0)
										red = 0;

									if ((green -= 4) < 0)
										green = 0;

									if ((blue += 6) >= 255)
										blue = 255;

									// store colors back
									color.peRed = red;
									color.peGreen = green;
									color.peBlue = blue;

									// set the color to a diminished intensity
									work_palette[pal_reg] = color;

								} // end for pal_reg


								// wait a bit

								Start_Clock(); Wait_Clock(12);

							} // end for index

	} break;

	case SCREEN_GREENNESS:
	{
							 // fade to green
							 for (index = 0; index < 64; index++)
							 {

								 // loop thru all palette registers
								 for (pal_reg = 0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
								 {
									 // get the entry data
									 color = work_palette[pal_reg];

									 // make 32 bit copy of color
									 red = color.peRed;
									 green = color.peGreen;
									 blue = color.peBlue;

									 if ((red -= 4) < 0)
										 red = 0;

									 if ((green += 6) >= 255)
										 green = 255;

									 if ((blue -= 4) < 0)
										 blue = 0;

									 // store colors back
									 color.peRed = red;
									 color.peGreen = green;
									 color.peBlue = blue;

									 // set the color to a diminished intensity
									 work_palette[pal_reg] = color;

								 } // end for pal_reg

								 // wait a bit

								 Start_Clock(); Wait_Clock(12);


							 } // end for index

	} break;

	case SCREEN_SWIPE_X:
	{
						   // do a screen wipe from right to left, left to right
						   for (index = 0; index < (m_nWidth / 2); index += 2)
						   {
							   // use this as a 1/70th of second time delay

							   Start_Clock(); Wait_Clock(12);

							   // test screen depth
							   if (m_nBPP == 8)
							   {
								   // draw two vertical lines at opposite ends of the screen
								   VLine(0, (m_nHeight - 1), (m_nWidth - 1) - index, 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), index, 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), (m_nWidth - 1) - (index + 1), 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), index + 1, 0, vbuffer, lpitch);
							   } // end if 8-bit mode
						   } // end for index

	} break;

	case SCREEN_SWIPE_Y:
	{
						   // do a screen wipe from top to bottom, bottom to top
						   for (index = 0; index < (m_nHeight / 2); index += 2)
						   {
							   // use this as a 1/70th of second time delay

							   Start_Clock(); Wait_Clock(12);

							   // test screen depth             
							   if (m_nBPP == 8)
							   {
								   // draw two horizontal lines at opposite ends of the screen
								   HLine(0, (m_nWidth - 1), (m_nHeight - 1) - index, 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), index, 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), (m_nHeight - 1) - (index + 1), 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), index + 1, 0, vbuffer, lpitch);
							   } // end if 8-bit mode
						   } // end for index


	} break;

	case SCREEN_SCRUNCH:
	{
						   // do a screen wipe from top to bottom, bottom to top
						   for (index = 0; index < (m_nWidth / 2); index += 2)
						   {
							   // use this as a 1/70th of second time delay

							   Start_Clock(); Wait_Clock(12);

							   // test screen depth             
							   if (m_nBPP == 8)
							   {
								   // draw two horizontal lines at opposite ends of the screen
								   HLine(0, (m_nWidth - 1), (m_nHeight - 1) - index % (m_nHeight / 2), 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), index % (m_nHeight / 2), 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), (m_nHeight - 1) - (index % (m_nHeight / 2) + 1), 0, vbuffer, lpitch);
								   HLine(0, (m_nWidth - 1), index % (m_nHeight / 2) + 1, 0, vbuffer, lpitch);

								   // draw two vertical lines at opposite ends of the screen
								   VLine(0, (m_nHeight - 1), (m_nWidth - 1) - index, 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), index, 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), (m_nWidth - 1) - (index + 1), 0, vbuffer, lpitch);
								   VLine(0, (m_nHeight - 1), index + 1, 0, vbuffer, lpitch);
							   } // end if 8-bit mode
						   } // end for index

	} break;


	case SCREEN_DISOLVE:
	{
						   // disolve the screen by plotting zillions of little black dots

						   if (m_nBPP == 8)
						   for (index = 0; index <= m_nWidth*m_nHeight * 4; index++)
							   Draw_Pixel(rand() % m_nWidth, rand() % m_nHeight, 0, vbuffer, lpitch);
	} break;

	default:break;

	} // end switch

} // end Screen_Transitions

//////////////////////////////////////////////////////////////////////////////

int Collision_Test(int x1, int y1, int w1, int h1,
	int x2, int y2, int w2, int h2)
{
	// this function tests if the two rects overlap

	// get the radi of each rect
	int width1 = (w1 >> 1) - (w1 >> 3);
	int height1 = (h1 >> 1) - (h1 >> 3);

	int width2 = (w2 >> 1) - (w2 >> 3);
	int height2 = (h2 >> 1) - (h2 >> 3);

	// compute center of each rect
	int cx1 = x1 + width1;
	int cy1 = y1 + height1;

	int cx2 = x2 + width2;
	int cy2 = y2 + height2;

	// compute deltas
	int dx = abs(cx2 - cx1);
	int dy = abs(cy2 - cy1);

	// test if rects overlap
	if (dx < (width1 + width2) && dy < (height1 + height2))
		return(1);
	else
		// else no collision
		return(0);

} // end Collision_Test

///////////////////////////////////////////////////////////

int CDDraw::Color_Scan(int x1, int y1, int x2, int y2,
	UCHAR scan_start, UCHAR scan_end,
	UCHAR *scan_buffer, int scan_lpitch)
{
	// this function implements a crude collision technique
	// based on scanning for a range of colors within a rectangle

	// clip rectangle

	// x coords first    
	if (x1 >= m_nWidth)
		x1 = m_nWidth - 1;
	else
	if (x1 < 0)
		x1 = 0;

	if (x2 >= m_nWidth)
		x2 = m_nWidth - 1;
	else
	if (x2 < 0)
		x2 = 0;

	// now y-coords
	if (y1 >= m_nHeight)
		y1 = m_nHeight - 1;
	else
	if (y1 < 0)
		y1 = 0;

	if (y2 >= m_nHeight)
		y2 = m_nHeight - 1;
	else
	if (y2 < 0)
		y2 = 0;

	// scan the region
	scan_buffer += y1*scan_lpitch;

	for (int scan_y = y1; scan_y <= y2; scan_y++)
	{
		for (int scan_x = x1; scan_x <= x2; scan_x++)
		{
			if (scan_buffer[scan_x] >= scan_start && scan_buffer[scan_x] <= scan_end)
				return(1);
		} // end for x

		// move down a line
		scan_buffer += scan_lpitch;

	} // end for y

	// return failure
	return(0);

} // end Color_Scan


//////////////////////////////////////////////////////////////////////////////

void CDDraw::Draw_Top_Tri(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a triangle that has a flat top

	float dx_right,    // the dx/dy ratio of the right edge of line
		dx_left,     // the dx/dy ratio of the left edge of line
		xs, xe,       // the starting and ending points of the edges
		height;      // the height of the triangle

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	// destination address of next scanline
	UCHAR  *dest_addr = NULL;

	// test order of x1 and x2
	if (x2 < x1)
	{
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	} // end if swap

	// compute delta's
	height = y3 - y1;

	dx_left = (x3 - x1) / height;
	dx_right = (x3 - x2) / height;

	// set starting points
	xs = (float)x1;
	xe = (float)x2 + (float)0.5;

	// perform y clipping
	if (y1 < m_rcWindows.top)
	{
		// compute new xs and ys
		xs = xs + dx_left*(float)(-y1 + m_rcWindows.top);
		xe = xe + dx_right*(float)(-y1 + m_rcWindows.top);

		// reset y1
		y1 = m_rcWindows.top;

	} // end if top is off screen

	if (y3 > m_rcWindows.bottom)
		y3 = m_rcWindows.bottom;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= m_rcWindows.left && x1 <= m_rcWindows.right &&
		x2 >= m_rcWindows.left && x2 <= m_rcWindows.right &&
		x3 >= m_rcWindows.left && x3 <= m_rcWindows.right)
	{
		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			memset((UCHAR *)dest_addr + (unsigned int)xs,
				color, (unsigned int)(xe - xs + 1));

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

		} // end for

	} // end if no x clipping needed
	else
	{
		// clip x axis with slower version

		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			// do x clip
			left = (int)xs;
			right = (int)xe;

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

			// clip line
			if (left < m_rcWindows.left)
			{
				left = m_rcWindows.left;

				if (right < m_rcWindows.left)
					continue;
			}

			if (right > m_rcWindows.right)
			{
				right = m_rcWindows.right;

				if (left > m_rcWindows.right)
					continue;
			}

			memset((UCHAR  *)dest_addr + (unsigned int)left,
				color, (unsigned int)(right - left + 1));

		} // end for

	} // end else x clipping needed

} // end Draw_Top_Tri

/////////////////////////////////////////////////////////////////////////////

void CDDraw::Draw_Bottom_Tri(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a triangle that has a flat bottom

	float dx_right,    // the dx/dy ratio of the right edge of line
		dx_left,     // the dx/dy ratio of the left edge of line
		xs, xe,       // the starting and ending points of the edges
		height;      // the height of the triangle

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	// destination address of next scanline
	UCHAR  *dest_addr;

	// test order of x1 and x2
	if (x3 < x2)
	{
		temp_x = x2;
		x2 = x3;
		x3 = temp_x;
	} // end if swap

	// compute delta's
	height = y3 - y1;

	dx_left = (x2 - x1) / height;
	dx_right = (x3 - x1) / height;

	// set starting points
	xs = (float)x1;
	xe = (float)x1; // +(float)0.5;

	// perform y clipping
	if (y1 < m_rcWindows.top)
	{
		// compute new xs and ys
		xs = xs + dx_left*(float)(-y1 + m_rcWindows.top);
		xe = xe + dx_right*(float)(-y1 + m_rcWindows.top);

		// reset y1
		y1 = m_rcWindows.top;

	} // end if top is off screen

	if (y3 > m_rcWindows.bottom)
		y3 = m_rcWindows.bottom;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= m_rcWindows.left && x1 <= m_rcWindows.right &&
		x2 >= m_rcWindows.left && x2 <= m_rcWindows.right &&
		x3 >= m_rcWindows.left && x3 <= m_rcWindows.right)
	{
		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			memset((UCHAR  *)dest_addr + (unsigned int)xs,
				color, (unsigned int)(xe - xs + 1));

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

		} // end for

	} // end if no x clipping needed
	else
	{
		// clip x axis with slower version

		// draw the triangle

		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			// do x clip
			left = (int)xs;
			right = (int)xe;

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

			// clip line
			if (left < m_rcWindows.left)
			{
				left = m_rcWindows.left;

				if (right < m_rcWindows.left)
					continue;
			}

			if (right > m_rcWindows.right)
			{
				right = m_rcWindows.right;

				if (left > m_rcWindows.right)
					continue;
			}

			memset((UCHAR  *)dest_addr + (unsigned int)left,
				color, (unsigned int)(right - left + 1));

		} // end for

	} // end else x clipping needed

} // end Draw_Bottom_Tri

///////////////////////////////////////////////////////////////////////////////

void CDDraw::Draw_TriangleFP_2D(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a triangle on the destination buffer using fixed point
	// it decomposes all triangles into a pair of flat top, flat bottom

	int temp_x, // used for sorting
		temp_y,
		new_x;

	// test for h lines and v lines
	if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
		return;

	// sort p1,p2,p3 in ascending y order
	if (y2 < y1)
	{
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	// now we know that p1 and p2 are in order
	if (y3 < y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	// finally test y3 against y2
	if (y3 < y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;

	} // end if

	// do trivial rejection tests for clipping
	if (y3<m_rcWindows.top || y1>m_rcWindows.bottom ||
		(x1<m_rcWindows.left && x2<m_rcWindows.left && x3<m_rcWindows.left) ||
		(x1>m_rcWindows.right && x2>m_rcWindows.right && x3>m_rcWindows.right))
		return;

	// test if top of triangle is flat
	if (y1 == y2)
	{
		Draw_Top_TriFP(x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	} // end if
	else
	if (y2 == y3)
	{
		Draw_Bottom_TriFP(x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	} // end if bottom is flat
	else
	{
		// general triangle that's needs to be broken up along long edge
		new_x = x1 + (int)(0.5 + (float)(y2 - y1)*(float)(x3 - x1) / (float)(y3 - y1));

		// draw each sub-triangle
		Draw_Bottom_TriFP(x1, y1, new_x, y2, x2, y2, color, dest_buffer, mempitch);
		Draw_Top_TriFP(x2, y2, new_x, y2, x3, y3, color, dest_buffer, mempitch);

	} // end else

} // end Draw_TriangleFP_2D

/////////////////////////////////////////////////////////////

void CDDraw::Draw_Triangle_2D(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a triangle on the destination buffer
	// it decomposes all triangles into a pair of flat top, flat bottom

	int temp_x, // used for sorting
		temp_y,
		new_x;

	// test for h lines and v lines
	if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
		return;

	// sort p1,p2,p3 in ascending y order
	if (y2 < y1)
	{
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	// now we know that p1 and p2 are in order
	if (y3 < y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	// finally test y3 against y2
	if (y3 < y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;

	} // end if

	// do trivial rejection tests for clipping
	if (y3<m_rcWindows.top || y1>m_rcWindows.bottom ||
		(x1<m_rcWindows.left && x2<m_rcWindows.left && x3<m_rcWindows.left) ||
		(x1>m_rcWindows.right && x2>m_rcWindows.right && x3>m_rcWindows.right))
		return;

	// test if top of triangle is flat
	if (y1 == y2)
	{
		Draw_Top_Tri(x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	} // end if
	else
	if (y2 == y3)
	{
		Draw_Bottom_Tri(x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	} // end if bottom is flat
	else
	{
		// general triangle that's needs to be broken up along long edge
		new_x = x1 + (int)(0.5 + (float)(y2 - y1)*(float)(x3 - x1) / (float)(y3 - y1));

		// draw each sub-triangle
		Draw_Bottom_Tri(x1, y1, new_x, y2, x2, y2, color, dest_buffer, mempitch);
		Draw_Top_Tri(x2, y2, new_x, y2, x3, y3, color, dest_buffer, mempitch);

	} // end else

} // end Draw_Triangle_2D

////////////////////////////////////////////////////////////////////////////////

inline void CDDraw::Draw_QuadFP_2D(int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a 2D quadrilateral

	// simply call the triangle function 2x, let it do all the work
	Draw_TriangleFP_2D(x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	Draw_TriangleFP_2D(x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);

} // end Draw_QuadFP_2D

////////////////////////////////////////////////////////////////////////////////

void CDDraw::Draw_Top_TriFP(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{
	// this function draws a triangle that has a flat top using fixed point math

	int dx_right,    // the dx/dy ratio of the right edge of line
		dx_left,     // the dx/dy ratio of the left edge of line
		xs, xe,       // the starting and ending points of the edges
		height;      // the height of the triangle

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	UCHAR  *dest_addr;

	// test for degenerate
	if (y1 == y3 || y2 == y3)
		return;

	// test order of x1 and x2
	if (x2 < x1)
	{
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	} // end if swap

	// compute delta's
	height = y3 - y1;

	dx_left = ((x3 - x1) << FIXP16_SHIFT) / height;
	dx_right = ((x3 - x2) << FIXP16_SHIFT) / height;

	// set starting points
	xs = (x1 << FIXP16_SHIFT);
	xe = (x2 << FIXP16_SHIFT);

	// perform y clipping
	if (y1 < m_rcWindows.top)
	{
		// compute new xs and ys
		xs = xs + dx_left*(-y1 + m_rcWindows.top);
		xe = xe + dx_right*(-y1 + m_rcWindows.top);

		// reset y1
		y1 = m_rcWindows.top;

	} // end if top is off screen

	if (y3 > m_rcWindows.bottom)
		y3 = m_rcWindows.bottom;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= m_rcWindows.left && x1 <= m_rcWindows.right &&
		x2 >= m_rcWindows.left && x2 <= m_rcWindows.right &&
		x3 >= m_rcWindows.left && x3 <= m_rcWindows.right)
	{
		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			memset((UCHAR *)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT),
				color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

		} // end for

	} // end if no x clipping needed
	else
	{
		// clip x axis with slower version

		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			// do x clip
			left = ((xs + FIXP16_ROUND_UP) >> 16);
			right = ((xe + FIXP16_ROUND_UP) >> 16);

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

			// clip line
			if (left < m_rcWindows.left)
			{
				left = m_rcWindows.left;

				if (right < m_rcWindows.left)
					continue;
			}

			if (right > m_rcWindows.right)
			{
				right = m_rcWindows.right;

				if (left > m_rcWindows.right)
					continue;
			}

			memset((UCHAR  *)dest_addr + (unsigned int)left,
				color, (unsigned int)(right - left + 1));

		} // end for

	} // end else x clipping needed

} // end Draw_Top_TriFP

/////////////////////////////////////////////////////////////////////////////

void CDDraw::Draw_Bottom_TriFP(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int color,
	UCHAR *dest_buffer, int mempitch)
{

	// this function draws a triangle that has a flat bottom using fixed point math

	int dx_right,    // the dx/dy ratio of the right edge of line
		dx_left,     // the dx/dy ratio of the left edge of line
		xs, xe,       // the starting and ending points of the edges
		height;      // the height of the triangle

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	UCHAR  *dest_addr;

	if (y1 == y2 || y1 == y3)
		return;

	// test order of x1 and x2
	if (x3 < x2)
	{
		temp_x = x2;
		x2 = x3;
		x3 = temp_x;

	} // end if swap

	// compute delta's
	height = y3 - y1;

	dx_left = ((x2 - x1) << FIXP16_SHIFT) / height;
	dx_right = ((x3 - x1) << FIXP16_SHIFT) / height;

	// set starting points
	xs = (x1 << FIXP16_SHIFT);
	xe = (x1 << FIXP16_SHIFT);

	// perform y clipping
	if (y1 < m_rcWindows.top)
	{
		// compute new xs and ys
		xs = xs + dx_left*(-y1 + m_rcWindows.top);
		xe = xe + dx_right*(-y1 + m_rcWindows.top);

		// reset y1
		y1 = m_rcWindows.top;

	} // end if top is off screen

	if (y3 > m_rcWindows.bottom)
		y3 = m_rcWindows.bottom;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= m_rcWindows.left && x1 <= m_rcWindows.right &&
		x2 >= m_rcWindows.left && x2 <= m_rcWindows.right &&
		x3 >= m_rcWindows.left && x3 <= m_rcWindows.right)
	{
		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			memset((UCHAR *)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT),
				color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

		} // end for

	} // end if no x clipping needed
	else
	{
		// clip x axis with slower version

		// draw the triangle
		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch)
		{
			// do x clip
			left = ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
			right = ((xe + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

			// adjust starting point and ending point
			xs += dx_left;
			xe += dx_right;

			// clip line
			if (left < m_rcWindows.left)
			{
				left = m_rcWindows.left;

				if (right < m_rcWindows.left)
					continue;
			}

			if (right > m_rcWindows.right)
			{
				right = m_rcWindows.right;

				if (left > m_rcWindows.right)
					continue;
			}

			memset((UCHAR *)dest_addr + left,
				color, (right - left + 1));

		} // end for

	} // end else x clipping needed

} // end Draw_Bottom_TriFP


///////////////////////////////////////////////////////////////////////////////

int CDDraw::Find_Bounding_Box_Poly2D(CPolygon2D* poly,
	float &min_x, float &max_x,
	float &min_y, float &max_y)
{
	// this function finds the bounding box of a 2D polygon 
	// and returns the values in the sent vars

	// is this poly valid?
	if (poly->num_verts == 0)
		return(0);

	// initialize output vars (note they are pointers)
	// also note that the algorithm assumes local coordinates
	// that is, the poly verts are relative to 0,0
	max_x = max_y = min_x = min_y = 0;

	// process each vertex
	for (int index = 0; index < poly->num_verts; index++)
	{
		// update vars - run min/max seek
		if (poly->vlist[index].x > max_x)
			max_x = poly->vlist[index].x;

		if (poly->vlist[index].x < min_x)
			min_x = poly->vlist[index].x;

		if (poly->vlist[index].y > max_y)
			max_y = poly->vlist[index].y;

		if (poly->vlist[index].y < min_y)
			min_y = poly->vlist[index].y;

	} // end for index

	// return success
	return(1);

} // end Find_Bounding_Box_Poly2D

////////////////////////////////////////////////////////////////

void CDDraw::Draw_Filled_Polygon2D(CPolygon2D* poly, UCHAR *vbuffer, int mempitch)
{
	// this function draws a general n sided polygon 

	int ydiff1, ydiff2,         // difference between starting x and ending x
		xdiff1, xdiff2,         // difference between starting y and ending y
		start,                  // starting offset of line between edges
		length,                 // distance from edge 1 to edge 2
		errorterm1, errorterm2, // error terms for edges 1 & 2
		offset1, offset2,       // offset of current pixel in edges 1 & 2
		count1, count2,         // increment count for edges 1 & 2
		xunit1, xunit2;         // unit to advance x offset for edges 1 & 2

	// initialize count of number of edges drawn:
	int edgecount = poly->num_verts - 1;

	// determine which vertex is at top of polygon:

	int firstvert = 0;         // start by assuming vertex 0 is at top

	int min_y = poly->vlist[0].y; // find y coordinate of vertex 0

	for (int index = 1; index < poly->num_verts; index++)
	{
		// Search thru vertices
		if ((poly->vlist[index].y) < min_y)
		{
			// is another vertex higher?
			firstvert = index;
			min_y = poly->vlist[index].y;
		} // end if

	} // end for index

	// finding starting and ending vertices of first two edges:
	int startvert1 = firstvert;      // get starting vertex of edge 1
	int startvert2 = firstvert;      // get starting vertex of edge 2
	int xstart1 = poly->vlist[startvert1].x + poly->x0;
	int ystart1 = poly->vlist[startvert1].y + poly->y0;
	int xstart2 = poly->vlist[startvert2].x + poly->x0;
	int ystart2 = poly->vlist[startvert2].y + poly->y0;
	int endvert1 = startvert1 - 1;           // get ending vertex of edge 1

	if (endvert1 < 0)
		endvert1 = poly->num_verts - 1;    // check for wrap

	int xend1 = poly->vlist[endvert1].x + poly->x0;      // get x & y coordinates
	int yend1 = poly->vlist[endvert1].y + poly->y0;      // of ending vertices
	int endvert2 = startvert2 + 1;           // get ending vertex of edge 2

	if (endvert2 == (poly->num_verts))
		endvert2 = 0;  // Check for wrap

	int xend2 = poly->vlist[endvert2].x + poly->x0;      // get x & y coordinates
	int yend2 = poly->vlist[endvert2].y + poly->y0;      // of ending vertices

	// draw the polygon:

	while (edgecount > 0)
	{
		// continue drawing until all edges drawn
		offset1 = mempitch*ystart1 + xstart1;  // offset of edge 1
		offset2 = mempitch*ystart2 + xstart2;  // offset of edge 2

		// initialize error terms
		// for edges 1 & 2
		errorterm1 = 0;
		errorterm2 = 0;

		// get absolute value of
		if ((ydiff1 = yend1 - ystart1) < 0)
			ydiff1 = -ydiff1;

		// x & y lengths of edges
		if ((ydiff2 = yend2 - ystart2) < 0)
			ydiff2 = -ydiff2;

		if ((xdiff1 = xend1 - xstart1) < 0)
		{
			// get value of length
			xunit1 = -1;                    // calculate X increment
			xdiff1 = -xdiff1;
		} // end if
		else
		{
			xunit1 = 1;
		} // end else

		if ((xdiff2 = xend2 - xstart2) < 0)
		{
			// Get value of length
			xunit2 = -1;                   // calculate X increment
			xdiff2 = -xdiff2;
		} // end else
		else
		{
			xunit2 = 1;
		} // end else

		// choose which of four routines to use
		if (xdiff1 > ydiff1)
		{
			// if x length of edge 1 is greater than y length
			if (xdiff2 > ydiff2)
			{
				// if X length of edge 2 is greater than y length

				// increment edge 1 on X and edge 2 on X:
				count1 = xdiff1;    // count for x increment on edge 1
				count2 = xdiff2;    // count for x increment on edge 2

				while (count1 && count2)
				{
					// continue drawing until one edge is done
					// calculate edge 1:
					while ((errorterm1 < xdiff1) && (count1 > 0))
					{
						// finished w/edge 1?
						if (count1--)
						{
							// count down on edge 1
							offset1 += xunit1;  // increment pixel offset
							xstart1 += xunit1;
						} // end if

						errorterm1 += ydiff1; // increment error term

						if (errorterm1 < xdiff1)
						{  // if not more than XDIFF
							vbuffer[offset1] = (UCHAR)poly->color; // ...plot a pixel
						} // end if

					} // end while

					errorterm1 -= xdiff1; // if time to increment X, restore error term

					// calculate edge 2:

					while ((errorterm2 < xdiff2) && (count2 > 0))
					{
						// finished w/edge 2?
						if (count2--)
						{
							// count down on edge 2
							offset2 += xunit2;  // increment pixel offset
							xstart2 += xunit2;
						} // end if

						errorterm2 += ydiff2; // increment error term

						if (errorterm2 < xdiff2)
						{  // if not more than XDIFF
							vbuffer[offset2] = (UCHAR)poly->color;  // ...plot a pixel
						} // end if

					} // end while

					errorterm2 -= xdiff2; // if time to increment X, restore error term

					// draw line from edge 1 to edge 2:

					length = offset2 - offset1; // determine length of horizontal line

					if (length < 0)
					{ // if negative...
						length = -length;       // make it positive
						start = offset2;        // and set START to edge 2
					} // end if
					else
						start = offset1;     // else set START to edge 1

					for (int index = start; index < start + length + 1; index++)
					{  // From edge to edge...
						vbuffer[index] = (UCHAR)poly->color;         // ...draw the line
					} // end for index

					offset1 += mempitch;           // advance edge 1 offset to next line
					ystart1++;
					offset2 += mempitch;           // advance edge 2 offset to next line
					ystart2++;

				} // end if

			} // end if
			else
			{
				// increment edge 1 on X and edge 2 on Y:
				count1 = xdiff1;    // count for X increment on edge 1
				count2 = ydiff2;    // count for Y increment on edge 2

				while (count1 && count2)
				{  // continue drawing until one edge is done
					// calculate edge 1:
					while ((errorterm1 < xdiff1) && (count1 > 0))
					{ // finished w/edge 1?
						if (count1--)
						{
							// count down on edge 1
							offset1 += xunit1;  // increment pixel offset
							xstart1 += xunit1;
						} // end if

						errorterm1 += ydiff1; // increment error term

						if (errorterm1 < xdiff1)
						{  // If not more than XDIFF
							vbuffer[offset1] = (UCHAR)poly->color; // ...plot a pixel
						} // end if

					} // end while

					errorterm1 -= xdiff1; // If time to increment X, restore error term

					// calculate edge 2:
					errorterm2 += xdiff2; // increment error term

					if (errorterm2 >= ydiff2)
					{ // if time to increment Y...
						errorterm2 -= ydiff2;        // ...restore error term
						offset2 += xunit2;           // ...and advance offset to next pixel
						xstart2 += xunit2;
					} // end if

					count2--;

					// draw line from edge 1 to edge 2:

					length = offset2 - offset1; // determine length of horizontal line

					if (length < 0)
					{ // if negative...
						length = -length;       // ...make it positive
						start = offset2;        // and set START to edge 2
					} // end if
					else
						start = offset1;        // else set START to edge 1

					for (int index = start; index < start + length + 1; index++)  // from edge to edge
					{
						vbuffer[index] = (UCHAR)poly->color;         // ...draw the line
					} // end for index

					offset1 += mempitch;           // advance edge 1 offset to next line
					ystart1++;
					offset2 += mempitch;           // advance edge 2 offset to next line
					ystart2++;

				} // end while
			} // end if
		} // end if
		else
		{
			if (xdiff2 > ydiff2)
			{
				// increment edge 1 on Y and edge 2 on X:

				count1 = ydiff1;  // count for Y increment on edge 1
				count2 = xdiff2;  // count for X increment on edge 2

				while (count1 && count2)
				{  // continue drawing until one edge is done
					// calculate edge 1:

					errorterm1 += xdiff1; // Increment error term

					if (errorterm1 >= ydiff1)
					{  // if time to increment Y...
						errorterm1 -= ydiff1;         // ...restore error term
						offset1 += xunit1;            // ...and advance offset to next pixel
						xstart1 += xunit1;
					} // end if

					count1--;

					// Calculate edge 2:

					while ((errorterm2 < xdiff2) && (count2 > 0))
					{ // finished w/edge 1?
						if (count2--)
						{ // count down on edge 2
							offset2 += xunit2;  // increment pixel offset
							xstart2 += xunit2;
						} // end if

						errorterm2 += ydiff2; // increment error term

						if (errorterm2 < xdiff2)
						{  // if not more than XDIFF
							vbuffer[offset2] = (UCHAR)poly->color; // ...plot a pixel
						} // end if
					} // end while

					errorterm2 -= xdiff2;  // if time to increment X, restore error term

					// draw line from edge 1 to edge 2:

					length = offset2 - offset1; // determine length of horizontal line

					if (length < 0)
					{    // if negative...
						length = -length;  // ...make it positive
						start = offset2;   // and set START to edge 2
					} // end if
					else
						start = offset1;  // else set START to edge 1

					for (int index = start; index < start + length + 1; index++) // from edge to edge...
					{
						vbuffer[index] = (UCHAR)poly->color;      // ...draw the line
					} // end for index

					offset1 += mempitch;         // advance edge 1 offset to next line
					ystart1++;
					offset2 += mempitch;         // advance edge 2 offset to next line
					ystart2++;

				} // end if
			} // end if
			else
			{
				// increment edge 1 on Y and edge 2 on Y:
				count1 = ydiff1;  // count for Y increment on edge 1
				count2 = ydiff2;  // count for Y increment on edge 2

				while (count1 && count2)
				{
					// continue drawing until one edge is done
					// calculate edge 1:
					errorterm1 += xdiff1;  // increment error term

					if (errorterm1 >= ydiff1)
					{                           // if time to increment Y
						errorterm1 -= ydiff1;         // ...restore error term
						offset1 += xunit1;            // ...and advance offset to next pixel
						xstart1 += xunit1;
					} // end if

					count1--;

					// calculate edge 2:
					errorterm2 += xdiff2;            // increment error term

					if (errorterm2 >= ydiff2)
					{                           // if time to increment Y
						errorterm2 -= ydiff2;         // ...restore error term
						offset2 += xunit2;            // ...and advance offset to next pixel
						xstart2 += xunit2;
					} // end if

					--count2;

					// draw line from edge 1 to edge 2:

					length = offset2 - offset1;  // determine length of horizontal line

					if (length < 0)
					{
						// if negative...
						length = -length;        // ...make it positive
						start = offset2;         // and set START to edge 2
					} // end if
					else
						start = offset1;         // else set START to edge 1

					for (int index = start; index < start + length + 1; index++)
					{ // from edge to edge
						vbuffer[index] = (UCHAR)poly->color;   // ...draw the linee
					} // end for index

					offset1 += mempitch;            // advance edge 1 offset to next line
					ystart1++;
					offset2 += mempitch;            // advance edge 2 offset to next line
					ystart2++;

				} // end while

			} // end else

		} // end if

		// another edge (at least) is complete. Start next edge, if any.
		if (!count1)
		{                      // if edge 1 is complete...
			--edgecount;           // decrement the edge count
			startvert1 = endvert1;   // make ending vertex into start vertex
			--endvert1;            // and get new ending vertex

			if (endvert1 < 0)
				endvert1 = poly->num_verts - 1; // check for wrap

			xend1 = poly->vlist[endvert1].x + poly->x0;  // get x & y of new end vertex
			yend1 = poly->vlist[endvert1].y + poly->y0;
		} // end if

		if (!count2)
		{                     // if edge 2 is complete...
			--edgecount;          // decrement the edge count
			startvert2 = endvert2;  // make ending vertex into start vertex
			endvert2++;           // and get new ending vertex

			if (endvert2 == (poly->num_verts))
				endvert2 = 0;                // check for wrap

			xend2 = poly->vlist[endvert2].x + poly->x0;  // get x & y of new end vertex
			yend2 = poly->vlist[endvert2].y + poly->y0;

		} // end if

	} // end while

} // end Draw_Filled_Polygon2D

///////////////////////////////////////////////////////////////

int CDDraw::DrawPolygon2D(CPolygon2D* poly)
{
	return DrawPolygon2D(poly, GetBackBuffer(), GetBackPitch(), m_rcWindows);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CDDraw::Clip_Line(int &x1, int &y1, int &x2, int &y2)
{
	return Clip_Line(x1, y1, x2, y2, m_rcWindows);
}

void SoaringLoong::Graphics::CDDraw::DDrawFillBackSurface(COLORREF dwColor,CRect* rcClient /* = NULL */)
{
	DDrawFillSurface(m_pBackSurface, dwColor, rcClient);
}

LPDIRECTDRAWSURFACE7 SoaringLoong::Graphics::CDDraw::GetBackSurface()
{
	return m_pBackSurface;
}

LPBYTE SoaringLoong::Graphics::CDDraw::GetBackBuffer()
{
	return m_pBackBuffer;
}

int SoaringLoong::Graphics::CDDraw::GetBackPitch()
{
	return m_dwBackPitch;
}
