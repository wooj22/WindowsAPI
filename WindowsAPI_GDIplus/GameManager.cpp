#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "GameManager.h"
#include "Message.h"
#include "../Module/Time.h"
#include "../Module/Input.h"
#include "../Module/ConsoleRenderer.h"
#pragma comment(lib, "gdiplus.lib")

/*----------------------- global -------------------------*/
// Window Data
LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");
HWND hwnd;	   		 // Window Handle
MSG msg;			 // Window Message
int g_width = 1024;
int g_height = 768;

// HDC (Device Context) 
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// GDI+ 그래픽 객체
Gdiplus::Graphics* g_pBackBufferGraphics;

// BITMAP
HBITMAP g_BackBufferBitmap;

// GDI+ Token
ULONG_PTR g_GdiPlusToken;

// Character
enum CharacterState
{
	IDLE,
	WALK,
	ATTACK
};

CharacterState playerState = IDLE;
CharacterState prePlayerState = IDLE;

const int IDLE_SIZE = 4;
const int WALK_SIZE = 6;
const int ATTACK_SIZE = 8;

Gdiplus::Bitmap* playerIdleFrames[IDLE_SIZE];
Gdiplus::Bitmap* playerWalkFrames[WALK_SIZE];
Gdiplus::Bitmap* playerAttackFrames[ATTACK_SIZE];
float animationTimer = 0.0f;
float animationCycle = 0.17f;
int animationIndex = 0;




/*----------------------- Console Funtion Funtions -------------------------*/
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




/*----------------------- Animation Funtions -------------------------*/
/// Player Idle Animation
inline void PlayerIdleAnimation() {
	if (animationIndex > IDLE_SIZE-1) animationIndex = 0;

	g_pBackBufferGraphics->DrawImage(playerIdleFrames[animationIndex],
		g_width / 2 - (int)playerIdleFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerIdleFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;
	printf("IDLE - currentIndex [%d]\n", animationIndex);
}

/// Player Walk Animation
inline void PlayerWalkAnimation() {
	if (animationIndex > WALK_SIZE - 1)	animationIndex = 0;
	
	g_pBackBufferGraphics->DrawImage(playerWalkFrames[animationIndex],
		g_width / 2 - (int)playerWalkFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerWalkFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;
	printf("WALK - currentIndex [%d]\n", animationIndex);
}

/// Player Attack Animation
inline void PlayerAttackAnimation() {
	g_pBackBufferGraphics->DrawImage(playerAttackFrames[animationIndex],
		g_width / 2 - (int)playerAttackFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerAttackFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;
	printf("ATTACK - currentIndex [%d]\n", animationIndex);

	// 1회 재생 후 IDLE로 전환
	if (animationIndex == ATTACK_SIZE - 1) playerState = IDLE;
}




/*----------------------- GameManager Funtions -------------------------*/
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

	// 더블 버퍼링 초기화
	g_FrontBufferDC = GetDC(hwnd);
	g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
	g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, g_width, g_height);
	SelectObject(g_BackBufferDC, g_BackBufferBitmap);

	// GDI+ 초기화
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
	g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// 이미지 로드
	wchar_t filePath[256];
	// Idle
	for (int i = 0; i < IDLE_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/idle-%d.png", i + 1);
		playerIdleFrames[i] = new Gdiplus::Bitmap(filePath);
	}

	// Walk
	for (int i = 0; i < WALK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/walk-%d.png", i + 1);
		playerWalkFrames[i] = new Gdiplus::Bitmap(filePath);
	}

	// Attack
	for (int i = 0; i < ATTACK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/attack-A%d.png", i + 1);
		playerAttackFrames[i] = new Gdiplus::Bitmap(filePath);
	}
}

/// Update
inline void Update() {
	// message 처리
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)	// break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// time update
	Time::UpdateTime();
	animationTimer += Time::GetDeltaTime();
	Input::Update();

	// input 처리
	if (Input::IsKeyPressed('1')) {
		prePlayerState = playerState;
		playerState = IDLE;
		animationIndex = 0;
	}
	if (Input::IsKeyPressed('2')) {
		prePlayerState = playerState;
		playerState = WALK;
		animationIndex = 0;
	}
	if (Input::IsKeyPressed('3')) {
		prePlayerState = playerState;
		playerState = ATTACK;
		animationIndex = 0;
	}

	// Animation Freame Update
	if (animationTimer >= animationCycle) {
		animationIndex++;
		animationTimer = 0.0f;
	}
}

/// Render
inline void Render() {
	PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);

	// Animation Draw
	switch (playerState)
	{
	case IDLE:
		PlayerIdleAnimation();
		break;
	case WALK:
		PlayerWalkAnimation();
		break;
	case ATTACK:
		PlayerAttackAnimation();
		break;
	default:
		break;
	}

	BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
}

/// Clear
inline void Clear() {
	// GDI+ 해제
	delete* playerIdleFrames;
	delete* playerWalkFrames;
	delete* playerAttackFrames;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// 콘솔 출력 해제
	UninitConsole();
}



/*-------------------------- Main Program ----------------------------*/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow)
{
	Initalize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	Time::Initialize();
	animationTimer = 0.0f;
	animationIndex = 0;

	// Main Loop
	while (true)
	{
		Update();
		Render();
	}

	Clear();
	return (int)msg.wParam;
}