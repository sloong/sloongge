#include "stdafx.h"

#include "IUniversal.h"

#include "SLoongBitmap.h"

using namespace SoaringLoong::Graphics;

CBitmap::CBitmap()
{
	m_lpBuffer = NULL;
}


CBitmap::~CBitmap()
{

}

HRESULT CBitmap::Shutdown()
{
	SAFE_DELETE_ARR(m_lpBuffer);
	return S_OK;
}

HRESULT CBitmap::LoadBitmapFromFile(LPCTSTR strFileName)
{
	HANDLE nFileHandle;
	DWORD nSize = 0;

	LPVOID lpTempBuffer = NULL;

	nFileHandle = CreateFile(strFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == nFileHandle)
	{
		return GetLastError();
	}

	ReadFile(nFileHandle, &m_stFileHeader, sizeof(BITMAPFILEHEADER), &nSize, NULL);
	if (m_stFileHeader.bfType != BITMAP_ID)
	{
		CloseHandle(nFileHandle);
		return S_FALSE;
	}

	ReadFile(nFileHandle, &m_stInfoHeader, sizeof(BITMAPINFOHEADER), &nSize, NULL);

	SetFilePointer(nFileHandle, -(int)m_stInfoHeader.biSizeImage, NULL, FILE_END);
	if (m_stInfoHeader.biBitCount == 24 && m_stInfoHeader.biSizeImage != 0)
	{
		int nImageSize = m_stInfoHeader.biHeight * m_stInfoHeader.biWidth;
		SAFE_DELETE_ARR(m_lpBuffer);
		m_lpBuffer = new BYTE[nImageSize*sizeof(COLORREF)];
		if (NULL == m_lpBuffer)
		{
			CloseHandle(nFileHandle);
			return S_FALSE;
		}

		LPBYTE lpTemp = new BYTE[m_stInfoHeader.biSizeImage];

		ReadFile((HANDLE)nFileHandle, lpTemp, m_stInfoHeader.biSizeImage, &nSize, NULL);


		LPBYTE lpDes = m_lpBuffer;
		LPBYTE lpSrc = lpTemp;

		int nWidth = (m_stInfoHeader.biWidth*m_stInfoHeader.biBitCount + 31) / 32 * 4;
		int nW = m_stInfoHeader.biWidth * 3;

		for (int i = 0; i < nImageSize; i++)
		{
			LPCOLORREF color = (LPCOLORREF)lpDes;
			*color = _ARGB(0, *(lpSrc+2), *(lpSrc+1), *lpSrc);

			if (m_stFileHeader.bfOffBits != 0 && i != 0 && nW != nWidth &&
				i % m_stInfoHeader.biWidth == 0 ) // is a line end
			{
				lpSrc = lpTemp + nWidth*(i / m_stInfoHeader.biWidth);
			}
			else
			{
				lpSrc += 3;
			}
			lpDes += 4;
		}
		SAFE_DELETE_ARR(lpTemp);
	}
	else
	{
		return S_FALSE;
	}

	FlipBitmap(m_stInfoHeader.biWidth*sizeof(COLORREF), m_stInfoHeader.biHeight);
	CloseHandle(nFileHandle);

	return S_OK;
}

HRESULT CBitmap::FlipBitmap(int nNumLine, int nHeight)
{
	// this function is used to flip bottom-up .BMP images

	UCHAR *buffer; // used to perform the image processing
	int index;     // looping index

	// allocate the temporary buffer
	buffer = new BYTE[nNumLine*nHeight];
	if (!(buffer))
		return(0);

	// copy image to work area
	memcpy(buffer, m_lpBuffer, nNumLine*nHeight);

	// flip vertically
	for (index = 0; index < nHeight; index++)
		memcpy(&m_lpBuffer[((nHeight - 1) - index)*nNumLine], &buffer[index*nNumLine], nNumLine);

	// release the memory
	SAFE_DELETE_ARR(buffer);

	// return success
	return S_OK;
}


int CBitmap::Scan_Image_Bitmap(LPRECT rcBmp, LPDIRECTDRAWSURFACE7 lpdds, LPRECT rcSurface)
{
	// this function extracts a bitmap out of a bitmap file
	RECT rcBmpTemp = { 0 }, rcSurfaceTemp = { 0 };
	DDSURFACEDESC2 ddsd = { 0 };  //  direct draw surface description 
	// set size of the structure
	ddsd.dwSize = sizeof(ddsd);
	// lock the display surface
	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	int nBmpHeight = (int)this->m_stInfoHeader.biHeight;
	int nBmpWidth = (int)this->m_stInfoHeader.biWidth;
	int nSurfaceHeight = (int)ddsd.dwHeight;
	int nSurfaceWidth = (int)ddsd.dwWidth;

	if (NULL == rcBmp)
	{
		rcBmp = &rcBmpTemp;
	}
	if (NULL == rcSurface)
	{
		rcSurface = &rcSurfaceTemp;
	}

	rcBmpTemp.bottom = nBmpHeight - 1;
	rcBmpTemp.right = nBmpWidth - 1;
	rcSurfaceTemp.bottom = nSurfaceHeight - 1;
	rcSurfaceTemp.right = nSurfaceWidth - 1;

	if (rcBmp->bottom >= nBmpHeight)
	{
		rcBmp->bottom = nBmpHeight - 1;
	}
	if (rcBmp->right >= nBmpWidth)
	{
		rcBmp->right = nBmpWidth - 1;
	}
	if (rcSurface->bottom >= nSurfaceHeight)
	{
		rcSurface->bottom = nSurfaceHeight - 1;
	}
	if (rcSurface->right >= nSurfaceWidth)
	{
		rcSurface->right = nSurfaceWidth - 1;
	}


	COLORREF* source_ptr = (COLORREF*)this->m_lpBuffer;  // working pointers
	COLORREF* dest_ptr = (COLORREF*)ddsd.lpSurface;

	source_ptr += rcBmp->top*nBmpWidth + rcBmp->left;
	dest_ptr += rcSurface->top*nSurfaceWidth + rcSurface->left;

	int nHeight;
	int nWdith;
	if (rcSurface->right - rcSurface->left > rcBmp->right - rcBmp->left)
	{
		nWdith = rcBmp->right - rcBmp->left;
	}
	else
	{
		nWdith = rcSurface->right - rcSurface->left;
	}

	if (rcSurface->bottom - rcSurface->top > rcBmp->bottom - rcBmp->top)
	{
		nHeight = rcBmp->bottom - rcBmp->top;
	}
	else
	{
		nHeight = rcSurface->bottom - rcSurface->top;
	}

	for (int i = 0; i < nHeight; i++)
	{
		memcpy((void*)dest_ptr, (void*)source_ptr, nWdith * 4);

		dest_ptr += nSurfaceWidth;
		source_ptr += nBmpWidth;
	}


	// unlock the surface 
	lpdds->Unlock(NULL);

	// return success
	return(1);

} // end Scan_Image_Bitmap

int CBitmap::Scan_Image_Bitmap(LPDIRECTDRAWSURFACE7 lpdds, int cx, int cy)              // cell to scan image from
{
	// this function extracts a bitmap out of a bitmap file

	UCHAR *source_ptr,   // working pointers
		*dest_ptr;

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	// get the addr to destination surface memory

	// set size of the structure
	ddsd.dwSize = sizeof(ddsd);

	// lock the display surface
	lpdds->Lock(NULL,
		&ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);

	// compute position to start scanning bits from
	cx = cx*(ddsd.dwWidth + 1) + 1;
	cy = cy*(ddsd.dwHeight + 1) + 1;

	// extract bitmap data
	source_ptr = this->m_lpBuffer + cy*this->m_stInfoHeader.biWidth + cx;

	// assign a pointer to the memory surface for manipulation
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int index_y = 0; index_y < ddsd.dwHeight; index_y++)
	{
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, ddsd.dwWidth);

		// advance pointers
		dest_ptr += (ddsd.lPitch);
		source_ptr += this->m_stInfoHeader.biWidth;
	} // end for index_y

	// unlock the surface 
	lpdds->Unlock(NULL);

	// return success
	return(1);

} // end Scan_Image_Bitmap

LONG SoaringLoong::Graphics::CBitmap::GetBitmapHeight()
{
	return m_stInfoHeader.biHeight;
}

LONG SoaringLoong::Graphics::CBitmap::GetBitmapWidth()
{
	return m_stInfoHeader.biWidth;
}

int SoaringLoong::Graphics::CBitmap::GetBiemapBPP()
{
	return m_stInfoHeader.biBitCount;
}
