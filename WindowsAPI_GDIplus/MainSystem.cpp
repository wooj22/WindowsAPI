#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "Message.h"

#pragma comment(lib, "gdiplus.lib")

// Window Data
LPCTSTR g_szClassName = TEXT("������ Ŭ���� �̸�");
int g_width = 1024;
int g_height = 768;

// HDC (Device Context) 
HWND g_hWnd;
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// BITMAP
HBITMAP g_BackBufferBitmap;


/*----------------------- Funtions -------------------------*/
/// �ܼ�â ����
void InitConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"������ �޽��� �ܼ� �α�");
	printf("�ܼ� �α� ����...\n\n");
}

/// �ܼ�â ����
void UninitConsole()
{
	fclose(stdout);
	FreeConsole();
}



/*-------------------------- Main ----------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �ܼ�â ����
	InitConsole();

	// ���丮 ��� ����
	char szPath[MAX_PATH] = { 0, };
	::GetCurrentDirectoryA(MAX_PATH, szPath);

	// WNDCLASS ���
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

	// ������ ����
	HWND hwnd = CreateWindow(
		g_szClassName,
		TEXT("GDL plus �ִϸ��̼� ������"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// ���� ���۸� �ʱ�ȭ
	g_FrontBufferDC = GetDC(hwnd);
	g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
	g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, g_width, g_height);
	SelectObject(g_BackBufferDC, g_BackBufferBitmap);

	// GDI+ �ʱ�ȭ
	ULONG_PTR g_GdiPlusToken;
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
	Gdiplus::Graphics* g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// �̹��� �ε�
	Gdiplus::Bitmap* g_pImageBitmap = new Gdiplus::Bitmap(L"../Resource/PlayerAnimation/attack-A1.png");
	UINT witdh = g_pImageBitmap->GetWidth();
	UINT height = g_pImageBitmap->GetHeight();

	if (g_pImageBitmap->GetLastStatus() != Gdiplus::Ok)
	{
		MessageBox(hwnd, L"PNG ���� �ε� ����", L"����", MB_ICONERROR);
		PostQuitMessage(0);
	}

	// NonBlocking����� Game Loop
	MSG msg;
	while (true)
	{
		// �޽��� get -> ������ ���ν��� ����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ä���
		PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);

		// �׸���
		g_pBackBufferGraphics->DrawImage(g_pImageBitmap,
			g_width / 2 - (int)g_pImageBitmap->GetWidth() / 2, g_height / 2 - (int)g_pImageBitmap->GetHeight() / 2);

		// ���� ����ϱ�
		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	// GDI+ ����
	delete g_pImageBitmap;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// �ܼ� ��� ����
	UninitConsole();  
	return (int)msg.wParam;
}
