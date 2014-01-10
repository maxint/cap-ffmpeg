// videoinput_demo.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <ffmpeg_cap.h>
#include "resource.h"
#include <tchar.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                        // current instance
TCHAR szTitle[MAX_LOADSTRING];          // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];    // The title bar text
LONG nTimer;

VideoCapture_FFMPEG* cap = NULL;
int nWidth = 0;
int nHeight = 0;
unsigned char *data[4] = {0};
int step[4] = {0};
BITMAPINFO bmi;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
    HACCEL hAccelTable;

    if (__argc != 2)
    {
        MessageBox(NULL, "usage: <EXE> <VIDEO FILE>", "Help", MB_OK | MB_ICONSTOP);
        return -1;
    }

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    _stprintf(szTitle, "%s - %s", szTitle, __argv[1]);
	LoadString(hInstance, IDS_WINDOW_CLASS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_DSHOW_CAP_DEMO);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDS_WINDOW_CLASS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
	case WM_CREATE:
        cap = ff_cap_create(__argv[1]);
        if (!cap) {
            MessageBox(hWnd, "Can not open given video file", "Error", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
            break;
        }
        nWidth  = (int)ff_cap_get(cap,FFMPEG_CAP_PROP_FRAME_WIDTH);
        nHeight = (int)ff_cap_get(cap,FFMPEG_CAP_PROP_FRAME_HEIGHT);
		nTimer = SetTimer(hWnd, 1, 30, NULL);

        SetWindowPos(hWnd, 0, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		//
		// Initialize the BITMAPINFO.
		//
		
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       = nWidth;
		bmi.bmiHeader.biHeight      = nHeight; // top-down image
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage   = 0;
        break;

    case WM_DESTROY:
        KillTimer(hWnd, nTimer);
        ff_cap_release(&cap);
        PostQuitMessage(0);
        break;

	case WM_TIMER:
		if (ff_cap_grab(cap) && ff_cap_retrieve(cap, (unsigned char**)data, (int *)step)) {
			InvalidateRect(hWnd, NULL, FALSE);
		}

	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

        case IDM_RELOAD:
            ff_cap_set(cap, FFMPEG_CAP_PROP_POS_MSEC, 0);
            SetWindowPos(hWnd, 0, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
        {
            RECT rt;
            HDC hdc;
            PAINTSTRUCT ps;

            hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            GetClientRect(hWnd, &rt);
            //SetStretchBltMode(hdc, MAXSTRETCHBLTMODE);
            SetStretchBltMode(hdc, COLORONCOLOR);
            StretchDIBits(hdc, 0, rt.bottom-1, rt.right, -rt.bottom, 
                0, 0, nWidth, nHeight, data[0], &bmi, DIB_RGB_COLORS, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
		break;

	case WM_ERASEBKGND:
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
