// videoinput_demo.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <ffmpeg_cap.h>
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <io.h>
#include <fcntl.h>

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

typedef VideoCapture_FFMPEG* (*ff_cap_create_t)(const char* fname);
typedef void    (*ff_cap_release_t)(VideoCapture_FFMPEG** cap);
typedef double  (*ff_cap_get_t)(VideoCapture_FFMPEG* cap, int propid);
typedef int     (*ff_cap_set_t)(VideoCapture_FFMPEG* cap, int propid, double val);
typedef int     (*ff_cap_grab_t)(VideoCapture_FFMPEG* cap);
typedef int     (*ff_cap_retrieve_t)(VideoCapture_FFMPEG* cap, const unsigned char* data[4], int step[4]);

ff_cap_create_t     ff_cap_create_f = 0;
ff_cap_release_t    ff_cap_release_f = 0;
ff_cap_get_t        ff_cap_get_f = 0;
ff_cap_set_t        ff_cap_set_f = 0;
ff_cap_grab_t       ff_cap_grab_f = 0;
ff_cap_retrieve_t   ff_cap_retrieve_f = 0;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void ReportWindowError(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
                                       (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR) lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    //ExitProcess(dw);
}

//
//  FUNCTION: RegisterWindowClass()
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
ATOM RegisterWindowClass(HINSTANCE hInstance)
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
        ReportWindowError(TEXT("CreateWindow"));
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
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
        cap = ff_cap_create_f(__argv[1]);
        if (!cap) {
            MessageBox(hWnd, "Can not open given video file", "Error", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
            break;
        }
        nWidth  = (int) ff_cap_get_f(cap, FFMPEG_PROP_FRAME_WIDTH);
        nHeight = (int) ff_cap_get_f(cap, FFMPEG_PROP_FRAME_HEIGHT);
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
        ff_cap_release_f(&cap);
        PostQuitMessage(0);
        break;

	case WM_TIMER:
		if (ff_cap_grab_f(cap) && ff_cap_retrieve_f(cap, data, step)) {
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
            ff_cap_set_f(cap, FFMPEG_PROP_POS_MSEC, 0);
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

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    //{
    //    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    //    // allocate a console for this app
    //    AllocConsole();
    //    // set the screen buffer to be big enough to let us scroll text
    //    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    //    coninfo.dwSize.Y = 999;
    //    // How many lines do you want to have in the console buffer
    //    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
    //}
    //{
    //    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    //    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    //    FILE* hf_out = _fdopen(hCrt, "w");
    //    setvbuf(hf_out, NULL, _IONBF, 0);
    //    *stdout = *hf_out;
    //}
    //{
    //    HANDLE handle_out = GetStdHandle(STD_ERROR_HANDLE);
    //    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    //    FILE* hf_out = _fdopen(hCrt, "w");
    //    setvbuf(hf_out, NULL, _IONBF, 0);
    //    *stderr = *hf_out;
    //}
    //{
    //    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    //    int hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    //    FILE* hf_in = _fdopen(hCrt, "r");
    //    setvbuf(hf_in, NULL, _IONBF, 0);
    //    *stdin = *hf_in;
    //}

 	// TODO: Place code here.
    MSG msg = {0};
    HACCEL hAccelTable = {0};
    HMODULE hModule = 0;

    if (__argc != 2)
    {
        MessageBox(NULL, TEXT("usage: <EXE> <VIDEO FILE>"), TEXT("Help"), MB_OK | MB_ICONSTOP);
        return -1;
    }

    hModule = LoadLibrary("ffmpeg_cap.dll");
    if (0 == hModule)
    {
        MessageBox(NULL, TEXT("Can not load ffmpeg_cap.dll"), TEXT("Error"), MB_OK | MB_ICONERROR);
        goto EXIT;
    }

    ff_cap_create_f     = (ff_cap_create_t  ) GetProcAddress(hModule, TEXT("ff_cap_create"));
    ff_cap_release_f    = (ff_cap_release_t ) GetProcAddress(hModule, TEXT("ff_cap_release"));
    ff_cap_get_f        = (ff_cap_get_t     ) GetProcAddress(hModule, TEXT("ff_cap_get"));
    ff_cap_set_f        = (ff_cap_set_t     ) GetProcAddress(hModule, TEXT("ff_cap_set"));
    ff_cap_grab_f       = (ff_cap_grab_t    ) GetProcAddress(hModule, TEXT("ff_cap_grab"));
    ff_cap_retrieve_f   = (ff_cap_retrieve_t) GetProcAddress(hModule, TEXT("ff_cap_retrieve"));

    if (ff_cap_create_f == 0 || ff_cap_release_f == 0 || ff_cap_get_f == 0 ||
        ff_cap_set_f == 0 || ff_cap_grab_f == 0 || ff_cap_retrieve_f == 0)
    {
        MessageBox(NULL, TEXT("Can not find enough process address in ffmpeg_cap.dll"), TEXT("Error"), MB_OK | MB_ICONERROR);
        goto EXIT;
    }

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    StringCchPrintf(szTitle, MAX_LOADSTRING, TEXT("%s - %s"), szTitle, __argv[1]);
	LoadString(hInstance, IDS_WINDOW_CLASS, szWindowClass, MAX_LOADSTRING);
	if (!RegisterWindowClass(hInstance))
    {
        ReportWindowError(TEXT("RegisterClassEx"));
        goto EXIT;
    }

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
        goto EXIT;
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

EXIT:
    if (hModule)
        FreeLibrary(hModule);

    FreeConsole();

	return msg.wParam;
}
