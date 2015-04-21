#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif

// DEFINES ////////////////////////////////////////////////
#include "IUniversal.h"
// default screen values, these are all overriden by the 
// call to DDraw_Init() and are just here to have something
// to set the globals to instead of constant values
#define MAX_COLORS_PALETTE  256

#define DEFAULT_PALETTE_FILE "PALDATA2.PAL"

// directdraw pixel format defines, used to help
// bitmap loader put data in proper format
#define DD_PIXEL_FORMAT8        8
#define DD_PIXEL_FORMAT555      15
#define DD_PIXEL_FORMAT565      16
#define DD_PIXEL_FORMAT888      24
#define DD_PIXEL_FORMATALPHA888 32 


// screen transition commands
#define SCREEN_DARKNESS  0         // fade to black
#define SCREEN_WHITENESS 1         // fade to white
#define SCREEN_SWIPE_X   2         // do a horizontal swipe
#define SCREEN_SWIPE_Y   3         // do a vertical swipe
#define SCREEN_DISOLVE   4         // a pixel disolve
#define SCREEN_SCRUNCH   5         // a square compression
#define SCREEN_BLUENESS  6         // fade to blue
#define SCREEN_REDNESS   7         // fade to red
#define SCREEN_GREENNESS 8         // fade to green

// defines for Blink_Colors
#define BLINKER_ADD           0    // add a light to database  
#define BLINKER_DELETE        1    // delete a light from database
#define BLINKER_UPDATE        2    // update a light
#define BLINKER_RUN           3    // run normal


// fixed point mathematics constants
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000

// initializes a direct draw struct, basically zeros it and sets the dwSize field
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }



// TYPES //////////////////////////////////////////////////
#include "SloongPolygon.h"
using SoaringLoong::Math::Polygon::CPolygon2D;

namespace SoaringLoong
{
	class CRect;
	using namespace SoaringLoong::Math;
	namespace Graphics
	{
		// blinking light structure
		typedef struct BLINKER_TYP
		{
			// user sets these
			int color_index;         // index of color to blink
			PALETTEENTRY on_color;   // RGB value of "on" color
			PALETTEENTRY off_color;  // RGB value of "off" color
			int on_time;             // number of frames to keep "on" 
			int off_time;            // number of frames to keep "off"

			// internal member
			int counter;             // counter for state transitions
			int state;               // state of light, -1 off, 1 on, 0 dead
		} BLINKER, *BLINKER_PTR;

	
		class SLOONGENGINE_API CDDraw
		{
		public:
			CDDraw();
			~CDDraw();
		
		public:// Static function
			
			static HRESULT DrawClipLine(int x0, int y0, int x1, int y1, COLORREF color, LPBYTE lpVideoBuffer, int lpitch, RECT rcScreen);
			static int Clip_Line(int &x1, int &y1, int &x2, int &y2, RECT rcScreen);
			static int Draw_Line(int xo, int yo, int x1, int y1, COLORREF color, LPBYTE vb_start, int lpitch);
			int DrawPolygon2D(CPolygon2D* poly, LPBYTE vbuffer, int lpitch, RECT rcScreen);
			
		
		public:// Member function
			// Init function
			int Initialize(HWND hWnd, int width, int height, int bpp, bool bFullScreen = false);
			
			// Create function
			LPDIRECTDRAWSURFACE7 CreateSurface(int width, int height, int mem_flags = 0, COLORREF color_key_value = 0);

			//  BackSurface function
			int DDrawFillSurface(LPDIRECTDRAWSURFACE7 lpDDrawSurface, COLORREF nColor, CRect* rcClient = NULL);
			void DDrawFillBackSurface(COLORREF dwColor, CRect* rcClient = NULL);
			LPDIRECTDRAWSURFACE7 GetBackSurface();
			LPBYTE GetBackBuffer();
			int GetBackPitch();

			// Draw function
			HRESULT DrawClipLine(int x0, int y0, int x1, int y1, COLORREF color);
			int Clip_Line(int &x1, int &y1, int &x2, int &y2);
			int DrawPolygon2D(CPolygon2D* poly);

		public:
			// TODO:: 还未整理的函数

			
			int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, int x, int y, int width, int height, LPDIRECTDRAWSURFACE7 dest, int transparent = 1);
			void PlotPixel(int x, int y, COLORREF dwColor, LPVOID pVideoBuffer, int nLpitch);


			LPDIRECTDRAWCLIPPER DDrawAttachClipper(LPDIRECTDRAW7 lpDDraw, LPDIRECTDRAWSURFACE7 lpDDrawSurface, int nNumRect, LPRECT lpRectList);
			// DirectDraw functions

			int DDraw_Shutdown(void);

			LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);


			int DDraw_Flip(void);
			int DDraw_Wait_For_Vsync(void);


			UCHAR *DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch);
			int DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds);

			UCHAR *DDraw_Lock_Primary_Surface(void);
			HRESULT DDraw_Unlock_Primary_Surface(void);

			UCHAR *DDraw_Lock_Back_Surface(void);
			HRESULT DDraw_Unlock_Back_Surface(void);

			// general utility functions
			DWORD Get_Clock(void);
			DWORD Start_Clock(void);
			DWORD Wait_Clock(DWORD count);

			int Collision_Test(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

			int Color_Scan(int x1, int y1, int x2, int y2, UCHAR scan_start, UCHAR scan_end,
				UCHAR *scan_buffer, int scan_lpitch);

			// graphics functions
			
			
			
			int Draw_Pixel(int x, int y, COLORREF color, LPBYTE video_buffer, int lpitch);
			int Draw_Rectangle(int x1, int y1, int x2, int y2, int color, LPDIRECTDRAWSURFACE7 lpdds);

			void HLine(int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch);
			void VLine(int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch);
			void Screen_Transitions(int effect, UCHAR *vbuffer, int lpitch);

			// palette functions

			int Rotate_Colors(int start_index, int end_index);
			int Blink_Colors(int command, BLINKER_PTR new_light, int id);

			// gdi functions
			int DrawText(LPCTSTR text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);

			// 2d 8-bit, 16-bit triangle rendering
			void Draw_Top_Tri(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

			void Draw_Bottom_Tri(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

			void Draw_Top_TriFP(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

			void Draw_Bottom_TriFP(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

			void Draw_Triangle_2D(int x1, int y1, int x2, int y2, int x3, int y3,
				int color, UCHAR *dest_buffer, int mempitch);

			void Draw_TriangleFP_2D(int x1, int y1, int x2, int y2, int x3, int y3,
				int color, UCHAR *dest_buffer, int mempitch);

			inline void Draw_QuadFP_2D(int x0, int y0, int x1, int y1,
				int x2, int y2, int x3, int y3,
				int color, UCHAR *dest_buffer, int mempitch);


			// general 2D 8-bit, 16-bit polygon rendering and transforming functions
			void Draw_Filled_Polygon2D(CPolygon2D* poly, UCHAR *vbuffer, int mempitch);

			

			// collision detection functions
			int Find_Bounding_Box_Poly2D(CPolygon2D* poly,
				float &min_x, float &max_x,
				float &min_y, float &max_y);

			// memory manipulation functions
			inline void Mem_Set_WORD(void *dest, USHORT data, int count);
			inline void Mem_Set_QUAD(void *dest, UINT   data, int count);

		protected:
			HWND					m_hWnd;
			// Width of screen.
			int m_nWidth;
			// height of screen
			int m_nHeight;
			// bits per pixel 
			int m_nBPP;
			// is this a windowed app?   
			bool m_bIsWindowed;
			SoaringLoong::CRect m_rcWindows;
			DWORD                start_clock_count;
			// DirectDraw Object
			LPDIRECTDRAW7			m_pDDraw;
			// Primary surface member variable
			LPDIRECTDRAWSURFACE7	m_pPrimarySurface;
			LPBYTE					m_pPrimaryBuffer;
			DWORD					m_dwPrimaryPitch;
			// Back surface member variables
			LPDIRECTDRAWSURFACE7	m_pBackSurface;
			LPBYTE					m_pBackBuffer;
			DWORD					m_dwBackPitch;
			// Status
			LPDIRECTDRAWCLIPPER		m_pWindowClipper;
			DDSURFACEDESC2			m_stSurfaceDesc;
		};
	}
}