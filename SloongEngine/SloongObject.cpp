#include "stdafx.h"
#include "SloongObject.h"
#include "IUniversal.h"
#include "SloongDraw.h"
#include "SloongBitmap.h"
#include "SloongGame.h"
#include "SloongException.h"
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

LPCTSTR strSpriteName = _T("SloongGUISprite");
LPCTSTR strTextFieldName = _T("SloongGUITextField");
LPCTSTR strButtonName = _T("SloongGUIButton");

tstring CObject::m_strTexturePath;

CObject::CObject(CDDraw* pDDraw)
{
	m_bIsDrawing = true;
	m_pDDraw = pDDraw;
	m_pBodyImageList = NULL;
	m_nBodyImageNum = MAX_BOB_FRAMES;
	m_rcObject = new CRect();

	min_clip_x = 0;
	max_clip_x = 0;
	min_clip_y = 0;
	max_clip_y = 0;
}


CObject::~CObject()
{
	SAFE_DELETE(m_rcObject);
	this->Destroy();
}

void CObject::SetID(UINT nID)
{
	m_nID = nID;
}

void CObject::SetTexture(const vector<tstring>& vTexture)
{
	m_vTexture.clear();
	m_vTexture = vTexture;

	CBitmap oBitmap;
	oBitmap.LoadBitmapFromFile(vTexture[0].c_str());
	LoadFrame(&oBitmap, 0, 0, 0, BITMAP_EXTRACT_MODE_ABS);
}

bool CObject::isDrawing() const
{
	return m_bIsDrawing;
}

void CObject::Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface)
{
	Draw(lpDDrawSurface);
}

void CObject::Enable(bool bEnable, bool bKeepDrawing)
{
	m_bEnable = bEnable;
	m_bKeepDrawing = bKeepDrawing;
}

bool CObject::isDisable() const
{
	return !m_bEnable;
}

RECT CObject::GetScrrenRect()
{
	return m_rcScreen;
}

void CObject::SetTexturePath(ctstring& strPath)
{
	m_strTexturePath = strPath;
}

tstring CObject::GetTexturePath()
{
	return m_strTexturePath;
}

UINT CObject::GetID() const
{
	return m_nID;
}

void CObject::SetPosition( const CRect& rcRect, float z)
{
	*m_rcObject = rcRect;
}

void CObject::SetFont(ctstring& strFontName, float fFontSize)
{

}

bool CObject::Update()
{
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(CSloongGame::GetAppMain()->m_hMainWnd, &pos);
	m_stStatus = NORMAL;
	if ( CSloongGame::InRect(pos,m_rcObject))
	{
		if (KEYDOWN(VK_LBUTTON))
		{
			m_stStatus = DOWN;
		}
		else
		{
			m_stStatus = HOVER;
		}

		switch (m_stStatus)
		{
		case SoaringLoong::Graphics::DOWN:
			CSloongGame::SendEvent(m_nID, UI_EVENT::BUTTON_DOWN);
			break;
		case SoaringLoong::Graphics::HOVER:
			CSloongGame::SendEvent(m_nID, UI_EVENT::HOVER_TIMED_START);
			break;
		case SoaringLoong::Graphics::UP:
			break;
		case SoaringLoong::Graphics::NORMAL:
			break;
		default:
			break;
		}
	}

	
	return true;
}


HRESULT CObject::Initialize(CDDraw* pDDraw, int x, int y, int width, int height, int nBodyImageNum, int attr,
	int mem_flags /* = 0 */, COLORREF dwColor /* = 0 */, int bpp /* = 32 */)
{
	m_pDDraw = pDDraw;
	m_nBodyImageNum = nBodyImageNum;

	m_pBodyImageList = new LPDIRECTDRAWSURFACE7[m_nBodyImageNum];
	animations = new LPINT[MAX_BOB_ANIMATIONS];

	// set state and attributes of BOB
	this->state = BOB_STATE_ALIVE;
	this->attr = attr;
	this->anim_state = 0;
	this->counter_1 = 0;
	this->counter_2 = 0;
	this->max_count_1 = 0;
	this->max_count_2 = 0;

	this->curr_frame = 0;
	this->bpp = bpp;
	this->curr_animation = 0;
	this->anim_counter = 0;
	this->anim_index = 0;
	this->anim_count_max = 0;
	this->m_rcObject->SetRect(x, y, width, height);
	this->xv = 0;
	this->yv = 0;

	// set dimensions of the new bitmap surface

	// set all images to null
	for (int i = 0; i < m_nBodyImageNum; i++)
		this->m_pBodyImageList[i] = NULL;

	// set all animations to null
	for (int i = 0; i < MAX_BOB_ANIMATIONS; i++)
		this->animations[i] = NULL;

	// now create each surface
	for (int i = 0; i < m_nBodyImageNum; i++)
	{
		auto pSurface = m_pDDraw->CreateSurface(this->m_rcObject->Width() + this->width_fill, this->m_rcObject->Height(), mem_flags,-1);
		if ( !pSurface )
		{
			return S_FALSE;
		}
		pDDraw->DDrawFillSurface(pSurface, dwColor);
		m_pBodyImageList[i] = pSurface;
	} // end for index

	// return success
	return S_OK;
}

HRESULT CObject::Clone(CObject* pSource)
{
	// this function clones a BOB and updates the attr var to reflect that
	// the BOB is a clone and not real, this is used later in the destroy
	// function so a clone doesn't destroy the memory of a real bob

	if (NULL == pSource)
	{
		return S_FALSE;
	}
	// copy the bob data
	this->anim_count_max = pSource->anim_count_max;
	this->state = pSource->state;
	anim_state = pSource->anim_state;
	attr = pSource->attr;

	*m_rcObject = *(pSource->m_rcObject);
	width_fill = pSource->width_fill;
	bpp = pSource->bpp;
	counter_1 = pSource->counter_1;
	counter_2 = pSource->counter_2;
	max_count_1 = pSource->max_count_1;
	max_count_2 = pSource->max_count_2;
	memcpy(varsI, pSource->varsI, sizeof(varsI)* 16);

	curr_frame = pSource->curr_frame;
	curr_animation = pSource->curr_animation;
	anim_counter = pSource->anim_counter;
	anim_index = pSource->anim_index;
	anim_count_max = pSource->anim_count_max;


	min_clip_x = pSource->min_clip_x;
	max_clip_x = pSource->max_clip_x;
	min_clip_y = pSource->min_clip_y;
	max_clip_y = pSource->max_clip_y;


	// 	screen_width = pSource->screen_width;
	// 	screen_height = pSource->screen_height;
	// 	screen_bpp = pSource->screen_bpp;
	// 	screen_windowed = pSource->screen_windowed;

	xv = pSource->xv;
	yv = pSource->yv;
	memcpy(varsF, pSource->varsF, sizeof(varsF)* 16);

	animations = pSource->animations;

	m_pDDraw = pSource->m_pDDraw;
	m_nBodyImageNum = pSource->m_nBodyImageNum;
	m_pBodyImageList = m_pBodyImageList;

	// set the clone attribute
	this->attr |= BOB_ATTR_CLONE;

	// return success
	return S_OK;

} // end Clone_BOB

///////////////////////////////////////////////////////////

int CObject::Destroy()
{
	// destroy the BOB, tests if this is a real bob or a clone
	// if real then release all the memory, otherwise, just resets
	// the pointers to null

	int index; // looping var


	// test if this is a clone
	if (attr && BOB_ATTR_CLONE)
	{
		// null link all surfaces
		for (index = 0; index < MAX_BOB_FRAMES; index++)
		if (m_pBodyImageList[index])
			m_pBodyImageList[index] = NULL;

		// release memory for animation sequences 
		for (index = 0; index < MAX_BOB_ANIMATIONS; index++)
		if (animations[index])
			animations[index] = NULL;

	} // end if
	else
	{
		// destroy each bitmap surface
		for (index = 0; index < MAX_BOB_FRAMES; index++)
		if (m_pBodyImageList[index])
			(m_pBodyImageList[index])->Release();

		// release memory for animation sequences 
		for (index = 0; index < MAX_BOB_ANIMATIONS; index++)
		if (animations[index])
			free(animations[index]);

	} // end else not clone

	// return success
	return(1);

} // end Destroy_BOB

///////////////////////////////////////////////////////////

void CObject::Draw(LPDIRECTDRAWSURFACE7 dest) // surface to draw the bob on
{
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest

	RECT source_rect; // the source rectangle                             

	// is bob visible
	if (!(this->attr & BOB_ATTR_VISIBLE))
		return;

	// fill in the source rect
	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = this->m_rcObject->Width();
	source_rect.bottom = this->m_rcObject->Height();
	
	HRESULT hRes = dest->Blt(m_rcObject, this->m_pBodyImageList[this->curr_frame], &source_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
	if ( FAILED(hRes) && m_bRenderError == false )
	{
		m_bRenderError = true;
		LPCTSTR ErrorString = DXGetErrorDescription(hRes);
		tstring str = CSloongGame::GetUniversal()->Format(_T("Render Error, object id : %d.Error string: %s.\r\n"), m_nID, ErrorString);
		throw CException(str.c_str());
	}

	/*// blt to destination surface
	for (int i = 0; i < m_nBodyImageNum; i++)
	{

	dest_rect.left = dest_rect.right;
	dest_rect.right = dest_rect.left + this->width;

	if (FAILED(dest->Blt(&dest_rect, this->m_pBodyImageList[i],
	&source_rect, (DDBLT_WAIT | DDBLT_KEYSRC),
	NULL)))
	return(0);

	}*/
} // end Draw_BOB

///////////////////////////////////////////////////////////

int CObject::DrawScaled(int swidth, int sheight,  // bob and new dimensions
	LPDIRECTDRAWSURFACE7 dest) // surface to draw the bob on)
{
	// this function draws a scaled bob to the size swidth, sheight

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             


	// is bob visible
	if (!(this->attr & BOB_ATTR_VISIBLE))
		return(1);

	CRect rcDest(m_rcObject);
	rcDest.right = rcDest.left + swidth;
	rcDest.bottom = rcDest.top + sheight;

	// fill in the source rect
	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = this->m_rcObject->Width();
	source_rect.bottom = this->m_rcObject->Height();

	// blt to destination surface
	if (FAILED(dest->Blt(&dest_rect, this->m_pBodyImageList[this->curr_frame],
		&source_rect, (DDBLT_WAIT | DDBLT_KEYSRC),
		NULL)))
		return(0);

	// return success
	return(1);
} // end Draw_Scaled_BOB

///////////////////////////////////////////////////////////

int CObject::LoadFrame(CBitmap* pBitmap, int frame, int cx, int cy, int mode)
{
	// this function extracts a bitmap out of a bitmap file

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 


	LPCOLORREF pSource;   // working pointers
	LPCOLORREF pDest;

	int nWidth = this->m_rcObject->Width();
	int nHeight = this->m_rcObject->Height();

	// test the mode of extraction, cell based or absolute
	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		// re-compute x,y
		cx = cx*(nWidth + 1) + 1;
		cy = cy*(nHeight + 1) + 1;
	} // end if

	// extract bitmap data
	pSource = (LPCOLORREF)pBitmap->m_lpBuffer + cy*pBitmap->m_stInfoHeader.biWidth + cx;

	// get the addr to destination surface memory

	// set size of the structure
	ddsd.dwSize = sizeof(ddsd);

	// lock the display surface
	m_pBodyImageList[frame]->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// assign a pointer to the memory surface for manipulation
	pDest = (LPCOLORREF)ddsd.lpSurface;

	int nCopyWidth = pBitmap->m_stInfoHeader.biWidth > nWidth ? nWidth : pBitmap->m_stInfoHeader.biWidth;
	int nCopyHeight = pBitmap->GetBitmapHeight() > nHeight ? nHeight : pBitmap->GetBitmapHeight();

	// iterate thru each scanline and copy bitmap
	for (int index_y = 0; index_y < nCopyHeight; index_y++)
	{
		// copy next line of data to destination
		memcpy(pDest, pSource, nCopyWidth*sizeof(COLORREF));

		// advance pointers
		pDest += nWidth;
		pSource += pBitmap->m_stInfoHeader.biWidth;
	} // end for index_y

	// unlock the surface 
	(this->m_pBodyImageList[frame])->Unlock(NULL);

	// set state to loaded
	this->attr |= BOB_ATTR_LOADED;

	// return success
	return(1);

} // end Load_Frame_BOB

int CObject::LoadFrame(CBitmap* pBitmap, int cx /* = 0 */, int cy /* = 0 */)
{
	// this function extracts a bitmap out of a bitmap file
	for (int i = 0; i < m_nBodyImageNum; i++)
	{
		LoadFrame(pBitmap, i, cx, cy, BITMAP_EXTRACT_MODE_CELL);
 		cx += this->m_rcObject->Width();
	}

	// return success
	return(1);

} // end Load_Frame_BOB


///////////////////////////////////////////////////////////

int CObject::Animate()
{
	// this function animates a bob, basically it takes a look at
	// the attributes of the bob and determines if the bob is 
	// a single frame, multiframe, or multi animation, updates
	// the counters and frames appropriately


	// test the level of animation
	if (this->attr & BOB_ATTR_SINGLE_FRAME)
	{
		// current frame always = 0
		this->curr_frame = 0;
		return(1);
	} // end if
	else
	if (this->attr & BOB_ATTR_MULTI_FRAME)
	{
		// update the counter and test if its time to increment frame
		if (++this->anim_counter >= this->anim_count_max)
		{
			// reset counter
			this->anim_counter = 0;

			// move to next frame
			if (++this->curr_frame >= this->m_nBodyImageNum)
				this->curr_frame = 0;

		} // end if

	} // end elseif
	else
	if (this->attr & BOB_ATTR_MULTI_ANIM)
	{
		// this is the most complex of the animations it must look up the
		// next frame in the animation sequence

		// first test if its time to animate
		if (++this->anim_counter >= this->anim_count_max)
		{
			// reset counter
			this->anim_counter = 0;

			// increment the animation frame index
			this->anim_index++;

			// extract the next frame from animation list 
			this->curr_frame = this->animations[this->curr_animation][this->anim_index];

			// is this and end sequence flag -1
			if (this->curr_frame == -1)
			{
				// test if this is a single shot animation
				if (this->attr & BOB_ATTR_ANIM_ONE_SHOT)
				{
					// set animation state message to done
					this->anim_state = BOB_STATE_ANIM_DONE;

					// reset frame back one
					this->anim_index--;

					// extract animation frame
					this->curr_frame = this->animations[this->curr_animation][this->anim_index];

				} // end if
				else
				{
					// reset animation index
					this->anim_index = 0;

					// extract first animation frame
					this->curr_frame = this->animations[this->curr_animation][this->anim_index];
				} // end else

			}  // end if

		} // end if

	} // end elseif

	// return success
	return(1);

} // end Amimate_BOB

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void CObject::Move()
{
	// this function moves the bob based on its current velocity
	// also, the function test for various motion attributes of the'
	// bob and takes the appropriate actions


	// translate the bob
	/*this->x += this->xv;
	this->y += this->yv;

	// test for wrap around
	if (this->attr & BOB_ATTR_WRAPAROUND)
	{
		// test x extents first
		if (this->x > max_clip_x)
			this->x = min_clip_x - this->width;
		else
		if (this->x < min_clip_x - this->width)
			this->x = max_clip_x;

		// now y extents
		if (this->x > max_clip_x)
			this->x = min_clip_x - this->width;
		else
		if (this->x < min_clip_x - this->width)
			this->x = max_clip_x;

	} // end if
	else
		// test for bounce
	if (this->attr & BOB_ATTR_BOUNCE)
	{
		// test x extents first
		if ((this->x > max_clip_x - this->width) || (this->x < min_clip_x))
			this->xv = -this->xv;

		// now y extents 
		if ((this->y > max_clip_y - this->height) || (this->y < min_clip_y))
			this->yv = -this->yv;

	} // end if*/

} // end Move_BOB

///////////////////////////////////////////////////////////

int CObject::LoadAnimation(int anim_index,
	int num_frames,
	int *sequence)
{
	// this function load an animation sequence for a bob
	// the sequence consists of frame indices, the function
	// will append a -1 to the end of the list so the display
	// software knows when to restart the animation sequence

	// allocate memory for bob animation
	if (!(this->animations[anim_index] = (int *)malloc((num_frames + 1)*sizeof(int))))
		return(0);

	// load data into 
	int index = 0;
	for (; index < num_frames; index++)
		this->animations[anim_index][index] = sequence[index];

	// set the end of the list to a -1
	this->animations[anim_index][index] = -1;

	// return success
	return(1);

} // end Load_Animation_BOB

///////////////////////////////////////////////////////////

void CObject::SetSpeed(int speed)
{
	// this function simply sets the animation speed of a bob

	// set speed
	this->anim_count_max = speed;

} // end Set_Anim_Speed

///////////////////////////////////////////////////////////

void CObject::SetAnimation(int anim_index)
{
	// this function sets the animation to play

	// set the animation index
	this->curr_animation = anim_index;

	// reset animation 
	this->anim_index = 0;

} // end Set_Animation_BOB

///////////////////////////////////////////////////////////

void CObject::SetVelocity(int xv, int yv)
{
	// this function sets the velocity of a bob

	// set velocity
	this->xv = xv;
	this->yv = yv;

} // end Set_Vel_BOB

///////////////////////////////////////////////////////////

void CObject::Hide()
{
	// this functions hides bob 

	// reset the visibility bit
	RESET_BIT(this->attr, BOB_ATTR_VISIBLE);

} // end Hide_BOB

///////////////////////////////////////////////////////////

void CObject::Show()
{
	// this function shows a bob
	// set the visibility bit
	SET_BIT(this->attr, BOB_ATTR_VISIBLE);

} // end Show_BOB

///////////////////////////////////////////////////////////

int CObject::Collision(CObject* pBody)
{
	int width = m_rcObject->Width();
	int height = m_rcObject->Height();
	// get the radi of each rect
	int width1 = (width >> 1) - (width >> 3);
	int height1 = (height >> 1) - (height >> 3);

	int width2 = (pBody->m_rcObject->Width() >> 1) - (pBody->m_rcObject->Width() >> 3);
	int height2 = (pBody->m_rcObject->Height() >> 1) - (pBody->m_rcObject->Height() >> 3);

	// compute center of each rect
	int cx1 = this->m_rcObject->left + width1;
	int cy1 = this->m_rcObject->top + height1;

	int cx2 = pBody->m_rcObject->left + width2;
	int cy2 = pBody->m_rcObject->top + height2;

	// compute deltas
	int dx = abs(cx2 - cx1);
	int dy = abs(cy2 - cy1);

	// test if rects overlap
	if (dx < (width1 + width2) && dy < (height1 + height2))
		return(1);
	else
		// else no collision
		return(0);

} // end Collision_BOBS