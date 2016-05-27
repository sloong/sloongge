#pragma once

extern SLOONGENGINE_API const wstring strSpriteName;
extern SLOONGENGINE_API const wstring strTextFieldName;
extern SLOONGENGINE_API const wstring strButtonName;

#define MAX_BOB_FRAMES         64   // maximum number of bob frames
#define MAX_BOB_ANIMATIONS     16   // maximum number of animation sequeces

#define BOB_ATTR_SINGLE_FRAME   1   // bob has single frame
#define BOB_ATTR_MULTI_FRAME    2   // bob has multiple frames
#define BOB_ATTR_MULTI_ANIM     4   // bob has multiple animations
#define BOB_ATTR_ANIM_ONE_SHOT  8   // bob will perform the animation once
#define BOB_ATTR_VISIBLE        16  // bob is visible
#define BOB_ATTR_BOUNCE         32  // bob bounces off edges
#define BOB_ATTR_WRAPAROUND     64  // bob wraps around edges
#define BOB_ATTR_LOADED         128 // the bob has been loaded
#define BOB_ATTR_CLONE          256 // the bob is a clone

#include <ddraw.h>
#include "univ/univ.h"
namespace Sloong
{
	namespace Graphics
	{
		class CDDraw;
		class CBitmap;
	}
	using namespace Universal;
	using namespace Graphics;
	namespace Graphics
	{
		typedef enum _Body_Status
		{
			BOB_STATE_DEAD,   // this is a dead bob
			BOB_STATE_ALIVE,   // this is a live bob
			BOB_STATE_DYING,   // this bob is dying
			BOB_STATE_ANIM_DONE,   // done animation state
		}BODY_STATUS;
		enum Button_Status
		{
			DOWN,
			HOVER,
			UP,
			NORMAL,
		};

		class CObject
		{
		public:
			CObject(CDDraw* pDDraw);
			~CObject();

		public:
			virtual void SetID(UINT nID);
			virtual UINT GetID() const;

			virtual void SetTexture(vector<wstring>* vTexture);
			virtual void SetPosition(const CRect& rcRect, float z);
			virtual void SetFont(const string& strFontName, float fFontSize);

			virtual bool isDrawing() const;
			virtual bool isDisable() const;
			virtual bool Update( const CPoint& MousePos );
			virtual void Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface);

			void Enable(bool bEnable, bool bKeepDrawing);

			RECT GetScrrenRect();

			HRESULT Initialize(CDDraw* pDDraw, int x, int y, int width, int height, int nBodyImageNum, int attr, int mem_flags = 0, COLORREF dwColor = 0, int bpp = 32);

			HRESULT Clone(CObject* pSource);
			int Destroy();
			void Draw(LPDIRECTDRAWSURFACE7 dest);
			int DrawScaled(int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);

			int LoadFrame(CBitmap* bitmap, int frame, int cx, int cy, int mode);
			int LoadFrame(CBitmap* bitmap, int cx, int cy);
			int Animate();
			void Move();
			int LoadAnimation(int anim_index, int num_frames, int *sequence);
			void SetVelocity(int xv, int yv);
			void SetSpeed(int speed);
			void SetAnimation(int anim_index);
			void Hide();
			void Show();
			int Collision(CObject* pBody);


			BODY_STATUS state;          // the state of the object (general)
			int anim_state;     // an animation state variable, up to you
			DWORD attr;           // attributes pertaining to the object (general)

			float xv, yv;          // velocity of object
			int width_fill;     // internal, used to force 8*x wide surfaces
			int bpp;            // bits per pixel
			int counter_1;      // general counters
			int counter_2;
			int max_count_1;    // general threshold values;
			int max_count_2;
			int varsI[16];      // stack of 16 integers
			float varsF[16];    // stack of 16 floats
			int curr_frame;     // current animation frame
			int curr_animation; // index of current animation
			int anim_counter;   // used to time animation transitions
			int anim_index;     // animation element index
			int anim_count_max; // number of cycles before animation
			LPINT* animations; // animation sequences
			// these defined the general clipping rectangle for software clipping
			int min_clip_x;                             // clipping rectangle 
			int	max_clip_x;
			int	min_clip_y;
			int	max_clip_y;

			int m_nBodyImageNum;
			LPDIRECTDRAWSURFACE7* m_pBodyImageList; // the bitmap images DD surfaces
			//

		public:
			static void SetTexturePath(const wstring& strPath);
			static const wstring& GetTexturePath();

		private:
			static wstring m_strTexturePath;

		protected:
			UINT m_nID;
			vector<wstring> m_vTexture;
			bool m_bIsDrawing;
			bool m_bEnable;
			bool m_bKeepDrawing;
			RECT m_rcScreen;
			CDDraw* m_pDDraw;
			bool m_bRenderError;
			CRect* m_rcObject;
			Button_Status m_stStatus;
		};
	}
}


