#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "GameManager.h"
#include "Message.h"
#include "../Module/Time.h"
#include "../Module/Input.h"
#include "../Module/ConsoleRenderer.h"
#include "../Module/GDIRenderer.h"

// Window Data
LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");
HWND hwnd;	   		 // Window Handle
MSG msg;			 // Window Message
int g_width = 1024;
int g_height = 768;

// Loop Flag
bool g_Loop = true;


/// 콘솔창 생성
inline void InitConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"윈도우 메시지 콘솔 로그");
	printf("콘솔 로그 시작...\n\n");
}

/// 콘솔창 해제
inline void UninitConsole()
{
	fclose(stdout);
	FreeConsole();
}


namespace GameManager {
	/// Initalize
	inline void Initalize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
		hwnd = CreateWindow(
			g_szClassName,
			TEXT("GDL plus 애니메이션 과제중"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			NULL, NULL, hInstance, NULL);

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		Time::Initialize();
		GDIRenderer::Initalize(hwnd, g_width, g_height);
	}

	/// Update
	inline void Update() {
		// message 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	g_Loop = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// time update
		Time::UpdateTime();
		Input::Update();
		GDIRenderer::Update();
	}

	/// Render
	inline void Render() {
		GDIRenderer::Render(g_width, g_height);
	}

	/// Clear
	inline void Release() {
		GDIRenderer::Release(hwnd);
		UninitConsole();
	}
}

/*-------------------------- Main Program ----------------------------*/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow)
{
	GameManager::Initalize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	while (g_Loop)
	{
		GameManager::Update();
		GameManager::Render();
	}

	GameManager::Release();
	return (int)msg.wParam;
}