#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "Message.h"

#pragma comment(lib, "gdiplus.lib")

// Window Data
LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");
int g_width = 1024;
int g_height = 768;

// HDC (Device Context) 
HWND g_hWnd;
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// BITMAP
HBITMAP g_BackBufferBitmap;


/*----------------------- Funtions -------------------------*/
/// 콘솔창 생성
void InitConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"윈도우 메시지 콘솔 로그");
	printf("콘솔 로그 시작...\n\n");
}

/// 콘솔창 해제
void UninitConsole()
{
	fclose(stdout);
	FreeConsole();
}



/*-------------------------- Main ----------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 콘솔창 생성
	InitConsole();

	// 디렉토리 경로 저장
	char szPath[MAX_PATH] = { 0, };
	::GetCurrentDirectoryA(MAX_PATH, szPath);

	// WNDCLASS 등록
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = Message::WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = g_szClassName;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClass(&wc);
	RECT rcClient = { 0, 0, (LONG)g_width, (LONG)g_height };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

	// 윈도우 생성
	HWND hwnd = CreateWindow(
		g_szClassName,
		TEXT("GDL plus 애니메이션 과제중"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// 더블 버퍼링 초기화
	g_FrontBufferDC = GetDC(hwnd);
	g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
	g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, g_width, g_height);
	SelectObject(g_BackBufferDC, g_BackBufferBitmap);

	// GDI+ 초기화
	ULONG_PTR g_GdiPlusToken;
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
	Gdiplus::Graphics* g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// 이미지 로드
	Gdiplus::Bitmap* g_pImageBitmap = new Gdiplus::Bitmap(L"../Resource/PlayerAnimation/attack-A1.png");
	UINT witdh = g_pImageBitmap->GetWidth();
	UINT height = g_pImageBitmap->GetHeight();

	if (g_pImageBitmap->GetLastStatus() != Gdiplus::Ok)
	{
		MessageBox(hwnd, L"PNG 파일 로드 실패", L"오류", MB_ICONERROR);
		PostQuitMessage(0);
	}

	// NonBlocking방식의 Game Loop
	MSG msg;
	while (true)
	{
		// 메시지 get -> 윈도우 프로시저 전달
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 채우기
		PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);

		// 그리기
		g_pBackBufferGraphics->DrawImage(g_pImageBitmap,
			g_width / 2 - (int)g_pImageBitmap->GetWidth() / 2, g_height / 2 - (int)g_pImageBitmap->GetHeight() / 2);

		// 복사 출력하기
		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	// GDI+ 해제
	delete g_pImageBitmap;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// 콘솔 출력 해제
	UninitConsole();  
	return (int)msg.wParam;
}
