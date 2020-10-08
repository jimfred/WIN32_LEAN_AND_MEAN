//Libraries: gdi32, comctl32
//Linker flags (gcc): -mwindows

#if defined(_WIN32) || defined(_WIN64)

#if !(defined(WINVER) && defined(_WIN32_WINNT))
	#include <sdkddkver.h>
	#undef WINVER
	#define WINVER _WIN32_WINNT_VISTA
	#undef _WIN32_WINNT
	#define _WIN32_WINNT _WIN32_WINNT_VISTA
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>

#include <stdio.h> // for printf


#define TBWIN_WIDTH_MIN 180
#define TBWIN_HEIGHT_MIN 160

#ifndef STATE_SYSTEM_UNAVAILABLE
#define STATE_SYSTEM_UNAVAILABLE 0x00000001
#endif
#ifndef OBJID_VSCROLL
#define OBJID_VSCROLL ((LONG)0xFFFFFFFB)
#endif
#ifndef OBJID_HSCROLL
#define OBJID_HSCROLL ((LONG)0xFFFFFFFA)
#endif

#define ID_EDIT 40000
#define ID_OK 40001





static LRESULT CALLBACK TextBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
	{
		LPCWSTR sText = ((CREATESTRUCT*)lParam)->lpCreateParams;

		if(!sText)
		{
			DestroyWindow(hWnd);
			return -1;
		}

		HWND hWndEdit = CreateWindowEx(0, WC_EDIT, sText, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | WS_BORDER,	// ES_OEMCONVERT ES_READONLY
								0, 0, 0, 0, hWnd, (HMENU)ID_EDIT, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

		HWND hWndOk = CreateWindowEx(0, WC_BUTTON, L"OK", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT,
								0, 0, 0, 0, hWnd, (HMENU)ID_OK, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

		if(!(hWndEdit && hWndOk))
		{
			DestroyWindow(hWnd);
			return -1;
		}

		HDC hDC = GetDC(hWndEdit);
		if(hDC)
		{
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			if (hFont)
			{
				SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				SendMessage(hWndOk, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				SelectObject(hDC, hFont);
			}

			RECT rect = {0};
			DrawText(hDC, sText, -1, &rect, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX | DT_EDITCONTROL);	// DT_EXTERNALLEADING
			ReleaseDC(hWndEdit, hDC);
			AdjustWindowRectEx(&rect, ((CREATESTRUCT*)lParam)->style, FALSE, ((CREATESTRUCT*)lParam)->dwExStyle);
			int iCX = GetSystemMetrics(SM_CXSCREEN)? GetSystemMetrics(SM_CXSCREEN) :140;
			int iCY = GetSystemMetrics(SM_CYSCREEN)? GetSystemMetrics(SM_CYSCREEN) :140;
			int iX = rect.right-rect.left < iCX - 120 ? rect.right-rect.left : iCX - 120;
			int iY = rect.bottom-rect.top < iCY - 120 ? rect.bottom-rect.top : iCY - 120;
			MoveWindow(hWnd, (iCX-iX)>>1, (iCY-iY)>>1, iX + 44, iY + 56, TRUE);
		}

	}
	return 0;
	//case WM_CTLCOLORSTATIC:{
		//HDC hdcStatic = (HDC)wParam;
		//SetTextColor(hdcStatic, RGB(0,0,0));
		//SetTextColor(hdcStatic, RGB(255,255,255));
		//SetBkColor(hdcStatic, RGB(0,0,0));
	//}return (LRESULT)GetStockObject(WHITE_BRUSH);
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_OK:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_SIZE:
	{
		HWND hWndEdit, hWndOk;
		SCROLLBARINFO sbInfo;

		hWndEdit = GetDlgItem(hWnd, ID_EDIT);
		hWndOk = GetDlgItem(hWnd, ID_OK);

		if(!(hWndEdit && hWndOk))
		{
			DestroyWindow(hWnd);
			return 0;
		}

		ShowScrollBar(hWndEdit, SB_BOTH, TRUE);
		MoveWindow(hWndEdit, 8, 4, LOWORD(lParam)-16, HIWORD(lParam)-38, TRUE);
		MoveWindow(hWndOk, LOWORD(lParam)-90, HIWORD(lParam)-28, 86, 22, TRUE);

		ZeroMemory(&sbInfo, sizeof(sbInfo));
		sbInfo.cbSize = sizeof(sbInfo);
		if(GetScrollBarInfo(hWndEdit, OBJID_HSCROLL, &sbInfo))
		{
			ShowScrollBar(hWndEdit, SB_HORZ, (sbInfo.rgstate[0] & STATE_SYSTEM_UNAVAILABLE ? FALSE : TRUE));
		}
		ZeroMemory(&sbInfo, sizeof(sbInfo));
		sbInfo.cbSize = sizeof(sbInfo);
		if(GetScrollBarInfo(hWndEdit, OBJID_VSCROLL, &sbInfo))
		{
			ShowScrollBar(hWndEdit, SB_VERT, (sbInfo.rgstate[0] &  STATE_SYSTEM_UNAVAILABLE ? FALSE : TRUE));
		}
	}
	break;
    case WM_DESTROY:
    	PostQuitMessage(0);
    	return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{

	const LPCWSTR sTitel = L"Titel: Show ExampleText";
	const LPCWSTR sText = L"asdfasdfdasdf asdf\r\nqwer qwer ";
	const int iWidth = 0;
	const int iHeight= 0;

	InitCommonControls();

	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = TextBoxProc;
	wcex.cbClsExtra = sizeof(&sText);
	wcex.hInstance = NULL;	//(HINSTANCE)GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	//(HBRUSH)(COLOR_3DFACE + 1) GetSysColorBrush(COLOR_3DFACE) (HBRUSH)GetStockObject(LTGRAY_BRUSH) (HBRUSH)(COLOR_WINDOW + 1)
	wcex.lpszClassName = L"TextBox";

	const ATOM atomClassID = RegisterClassEx(&wcex);
	if(!atomClassID){
		return 0;
	}

	const DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	const DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	RECT rect = { 0 };
	int iX = (GetSystemMetrics(SM_CXSCREEN)-(rect.right-rect.left))>>1;
	if(iX < 1)
	{
		iX = CW_USEDEFAULT;
	}
	
	int iY = (GetSystemMetrics(SM_CYSCREEN)-(rect.bottom-rect.top))>>1;
	if(iY < 1)
	{
		iY = CW_USEDEFAULT;
	}
	
	const HWND hWndD = CreateWindowEx( dwExStyle, MAKEINTATOM(atomClassID), sTitel, dwStyle,
							iX, iY, rect.right-rect.left, rect.bottom-rect.top,
							NULL, NULL, wcex.hInstance, sText);

	if(!hWndD){
		UnregisterClass(MAKEINTATOM(atomClassID), wcex.hInstance);
		return 0;
	}

	ShowWindow(hWndD, SW_SHOW);
	UpdateWindow(hWndD);
	SetForegroundWindow(hWndD);
	SetFocus(hWndD);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if(!IsDialogMessage(hWndD, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(MAKEINTATOM(atomClassID), wcex.hInstance);

	return 0;
}

#endif
