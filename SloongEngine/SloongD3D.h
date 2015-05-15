#pragma once

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
namespace SoaringLoong
{
	namespace DirectX
	{
		class CDInput;
		class CD3D
		{
		public:
			CD3D(){}
			~CD3D(){}

			void Init(HWND hWnd, HINSTANCE hInst);
			void Loop();

			static CDInput* GetDInput();

			HWND m_hWnd;
		};
	}
}