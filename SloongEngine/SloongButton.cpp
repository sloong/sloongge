#include "stdafx.h"
#include "SloongButton.h"
#include "graphics/SloongBitmap.h"

using namespace Sloong;
using namespace Sloong::Graphics;

CButton::CButton(CDDraw* pDDraw) : CObject(pDDraw)
{
	Initialize(pDDraw, 0, 0, 800, 600, 4, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY);
}


CButton::~CButton()
{
}

void Sloong::Graphics::CButton::SetTexture( vector<CString>* vTexture)
{
	m_vTexture.clear();
	m_vTexture = (*vTexture);

	CBitmap oBitmap;
	oBitmap.LoadBitmapFromFile(m_vTexture[0]);
	LoadFrame(&oBitmap, 0, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(m_vTexture[1]);
	LoadFrame(&oBitmap, 1, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(m_vTexture[2]);
	LoadFrame(&oBitmap, 2, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(m_vTexture[3]);
	LoadFrame(&oBitmap, 3, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();
}

void Sloong::Graphics::CButton::Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface)
{
	this->curr_frame = m_stStatus;
	CObject::Render(lpDDrawSurface);
}
