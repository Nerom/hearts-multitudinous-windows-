#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include <gdiplus.h>
#include <shellapi.h>		//托盘图标头文件
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib , "winmm.lib")//以上为添加gdi+库的代码

const int WND_WIDTH=GetSystemMetrics(SM_CXSCREEN);
const int WND_HEIGHT=GetSystemMetrics(SM_CYSCREEN);

#define WND_CLASSNAME	_T("MainWnd")
#define WND_TITLENAME	_T("Test")
#define	WM_ICON_NOTIFY	WM_APP+10
#define WNDW 70
#define WNDH 70
#define MAXSPEED	4
#define SZTIP			SZINFO
#define SZINFO			_T("Running！")
#define SZINFOTITLE		_T("Running!")

ULONG_PTR m_pGdiToken;
GdiplusStartupInput m_gdiplusStartupInput;//gdi+启动代码
HMENU hMenu;
LRESULT	CALLBACK	WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL RegWinClass(HINSTANCE hInstance);
void MessageBoxError();
void CALLBACK timerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);
HWND CreateWnd(HINSTANCE hInstance,HBITMAP hBmp);
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,HINSTANCE hInst,HWND hwnd,HBITMAP &hBmp);//从资源加载png图片
BOOL initialize(HINSTANCE hInstance);

NOTIFYICONDATA	notify={0};

typedef struct
{
	HWND hWnd;
	POINT point;
	int xs,ys,xop,yop;
	HBITMAP hbmp;
} MovingWnd;