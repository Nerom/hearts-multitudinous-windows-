#include "标头.h"

MovingWnd mw[8];


//------------------------------------------------------
//	程序入口
//------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow)
{
	
	
	if(!RegWinClass(hInstance))
	{
		MessageBoxError();
		return 0;
	}
	GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL);//启动gdi+
	srand(timeGetTime());

	initialize(hInstance);
	
	for(int i=0;i<8;i++)
	{
		mw[i].hWnd=CreateWnd(hInstance,mw[i].hbmp);
		ImageFromIDResource(IDB_PNG1+i,_T("PNG"),hInstance,mw[i].hWnd,mw[i].hbmp);
		HDC hdc=GetDC(mw[i].hWnd);
		HDC mHdc=CreateCompatibleDC(hdc);
		SelectObject(mHdc,mw[i].hbmp);
		BLENDFUNCTION mblend={0};
		mblend.BlendOp=AC_SRC_OVER;
		mblend.AlphaFormat=AC_SRC_ALPHA;
		mblend.BlendFlags=0;
		mblend.SourceConstantAlpha=150;
		SIZE Size={WNDW,WNDH};
		POINT p={0};
		if(!UpdateLayeredWindow(mw[i].hWnd,NULL,&p,&Size,mHdc,&p,0,&mblend,ULW_ALPHA))
			MessageBoxError();
		DeleteDC(mHdc);//删除内存dc
		ReleaseDC(mw[i].hWnd, hdc);//释放dc
		
		mw[i].xs=rand()%MAXSPEED+1;
		mw[i].ys=rand()%MAXSPEED+1;
		mw[i].xop=rand()%2;
		mw[i].yop=rand()%2;
		if(!mw[i].xop)
			mw[i].xop=-1;
		if(!mw[i].yop)
			mw[i].yop=-1;
		mw[i].point.x=rand()%WND_WIDTH;
		mw[i].point.y=rand()%WND_HEIGHT;
		SetTimer(mw[i].hWnd,i+1,20,timerProc);
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	Shell_NotifyIcon(NIM_DELETE,&notify);//删除托盘图标
	GdiplusShutdown(m_pGdiToken);//关闭gdi+
	UnregisterClass(WND_CLASSNAME, hInstance);
	return 0;
}
BOOL initialize(HINSTANCE hInstance)
{
	DWORD wndStyle = WS_POPUP;
	RECT rcWnd = {0};
	AdjustWindowRect(&rcWnd, wndStyle, FALSE);
	HWND hWnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		WND_CLASSNAME,
		NULL,
		wndStyle,
		(GetSystemMetrics(SM_CXSCREEN)-WND_WIDTH)/2,
		(GetSystemMetrics(SM_CYSCREEN)-WND_HEIGHT)/2,
		WND_WIDTH, WND_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	HICON hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//加载图标
	if(hIcon==NULL)
		MessageBox(NULL,_T("Icon or Menu load error!"),_T("Error!"),MB_OK);

	notify.cbSize=sizeof(notify);
	notify.dwInfoFlags=NIIF_USER | NIIF_LARGE_ICON ;
	notify.dwState=NIS_HIDDEN|NIS_SHAREDICON;
	notify.dwStateMask=NIS_HIDDEN|NIS_SHAREDICON;
	notify.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP ;//| NIF_INFO;
	notify.hIcon=hIcon;
	notify.uCallbackMessage=WM_ICON_NOTIFY;
	notify.hBalloonIcon=hIcon;
	notify.uTimeout=30000;
	notify.uVersion=NOTIFYICON_VERSION;
	_tcsncpy(notify.szInfo,SZINFO,sizeof(SZINFO));
	_tcsncpy(notify.szInfoTitle,SZINFOTITLE,sizeof(SZINFOTITLE));
	_tcsncpy(notify.szTip,SZTIP,sizeof(SZTIP));
	notify.hWnd=hWnd;
	if(!Shell_NotifyIcon(NIM_ADD,&notify))
	{
		MessageBoxError();
		exit(0);
	}//创建托盘图标
	//托盘显示的属性设置
	hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU));//加载菜单
	if(hMenu==NULL)
		MessageBoxError();
	return true;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//消息处理函数
{
	HMENU hSubMenu=NULL;
	switch(uMsg)
	{
	case WM_ICON_NOTIFY://托盘图标上的消息
		switch(LOWORD(lParam))
		{
		case WM_RBUTTONUP://右键
			POINT pos;
			hSubMenu=GetSubMenu(hMenu,0);
			GetCursorPos(&pos);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hSubMenu,0,pos.x,pos.y,0,hWnd,NULL);//在鼠标位置弹出菜单
			break;
		case WM_LBUTTONUP:
			SetForegroundWindow(hWnd);//激活该窗口
			break;
		case WM_LBUTTONDBLCLK://左键双击
			break;
		}
		break;
	case WM_COMMAND://命令消息
		int wmId    = LOWORD(wParam);
		switch(wmId)
		{
		case ID_EXIT:
			PostQuitMessage(0);//退出
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);//默认的消息处理函数
}


BOOL RegWinClass(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WND_CLASSNAME;

	return RegisterClass(&wc);
}


BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,HINSTANCE hInst,HWND hwnd,HBITMAP &hBmp)//从资源加载png图片
{
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
	if (!hRsrc)
		return FALSE;
	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;
	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);//将资源拷贝到内存的另一个区域，以便操作
	GlobalUnlock(m_hMem);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
	// load from stream

/*	pBitmap=Gdiplus::Bitmap::FromStream(pstm);
	pBitmap->GetHBITMAP(Gdiplus::Color(0,0,0,0),&hBitmap);
	HDC oldHDC=(HDC)SelectObject(mHdc,hBitmap);
	if(oldHDC==NULL)
		MessageBoxError();*/
	HDC hdc=GetDC(hwnd);
	HDC mHdc=CreateCompatibleDC(hdc);
	Image *pImg=Gdiplus::Image::FromStream(pstm);//加载内存的png图片，并返回一个指针
	BYTE* pBits = NULL; 
	BITMAPINFOHEADER bmih = {  sizeof (BITMAPINFOHEADER) };
	bmih.biWidth                =  WNDW; 
	bmih.biHeight               = WNDH ; 
	bmih.biPlanes               = 1 ; 
	bmih.biBitCount             = 32;        //这里一定要是32 
	bmih.biCompression          = BI_RGB ; 
	bmih.biSizeImage            = 0 ; 
	bmih.biXPelsPerMeter        = 0 ; 
	bmih.biYPelsPerMeter        = 0 ; 
	bmih.biClrUsed              = 0 ; 
	bmih.biClrImportant         = 0 ;
	hBmp = CreateDIBSection (NULL, (BITMAPINFO *)&bmih, DIB_RGB_COLORS, (VOID**)&pBits, NULL, 0) ;
	if(hBmp == NULL)
	{
		MessageBoxError();
		exit(0);
	}
	SelectObject(mHdc,hBmp);
	Graphics graphics(mHdc);//创建graphics绘图对象
	graphics.DrawImage(pImg,0,0,WNDW,WNDH);
	DeleteDC(mHdc);//删除内存dc
	ReleaseDC(hwnd,hdc);

	// free/release stuff
	pstm->Release();
	FreeResource(lpRsrc);
	GlobalFree(m_hMem);//释放流、资源、内存
	return TRUE;
}

void CALLBACK timerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{
	SetWindowPos(hWnd,HWND_TOP,mw[nTimerid-1].point.x,mw[nTimerid-1].point.y,WNDW,WNDH,SWP_NOACTIVATE);
	mw[nTimerid-1].point.x+=(mw[nTimerid-1].xs*mw[nTimerid-1].xop);
	mw[nTimerid-1].point.y+=(mw[nTimerid-1].ys*mw[nTimerid-1].yop);
	if (mw[nTimerid-1].point.y<=0)
	{
		mw[nTimerid-1].yop=1;
		mw[nTimerid-1].ys=rand()%MAXSPEED+1;
	}
	if (mw[nTimerid-1].point.x<=0)
	{
		mw[nTimerid-1].xop=1;
		mw[nTimerid-1].xs=rand()%MAXSPEED+1;
	}
	if (mw[nTimerid-1].point.y+WNDH>=WND_HEIGHT)
	{
		mw[nTimerid-1].yop=-1;
		mw[nTimerid-1].ys=rand()%MAXSPEED+1;
	}
	if (mw[nTimerid-1].point.x+WNDW>=WND_WIDTH)
	{
		mw[nTimerid-1].xop=-1;
		mw[nTimerid-1].xs=rand()%MAXSPEED+1;
	}//判断图片是否运动到屏幕边界，如果是，则运动方向反向，随机该方向上的运动速度
//	BringWindowToTop(hWnd);
}

HWND CreateWnd(HINSTANCE hInstance,HBITMAP hBmp)
{
	DWORD wndStyle = WS_POPUP;
	RECT rcWnd = { 0, 0, WNDW, WNDH };
	AdjustWindowRect(&rcWnd, wndStyle, FALSE);

	HWND hWnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		WND_CLASSNAME,
		NULL,
		wndStyle,
		(GetSystemMetrics(SM_CXSCREEN)-WND_WIDTH)/2,
		(GetSystemMetrics(SM_CYSCREEN)-WND_HEIGHT)/2,
		WNDW, WNDH,
		NULL,
		NULL,
		hInstance,
		NULL);
	if(!hWnd)
	{
		MessageBoxError();
		exit(0);
	}
	ShowWindow(hWnd,SW_SHOW);
	return hWnd;
}
void MessageBoxError()
{
	 LPVOID lpMsgBuf; 
     FormatMessage( 
     FORMAT_MESSAGE_ALLOCATE_BUFFER | 
     FORMAT_MESSAGE_FROM_SYSTEM | 
     FORMAT_MESSAGE_IGNORE_INSERTS, 
     NULL, 
     GetLastError(), 
     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
     (LPTSTR) &lpMsgBuf, 
     0, 
     NULL 
     ); 
    // Process any inserts in lpMsgBuf. 
     //显示错误信息 
     MessageBox(NULL,(LPCTSTR)lpMsgBuf,_T("ERROR!!"),MB_OK); 
     
     // Free the buffer. 
     LocalFree( lpMsgBuf );
}