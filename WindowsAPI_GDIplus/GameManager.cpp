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
LPCTSTR g_szClassName = TEXT("������ Ŭ���� �̸�");
HWND hwnd;	   		 // Window Handle
MSG msg;			 // Window Message
int g_width = 1024;
int g_height = 768;

// HDC (Device Context) 
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// GDI+ �׷��� ��ü
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
/// �ܼ�â ����
inline void InitConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"������ �޽��� �ܼ� �α�");
	printf("�ܼ� �α� ����...\n\n");
}

/// �ܼ�â ����
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

	// 1ȸ ��� �� IDLE�� ��ȯ
	if (animationIndex == ATTACK_SIZE - 1) playerState = IDLE;
}




/*----------------------- GameManager Funtions -------------------------*/
/// Initalize
inline void Initalize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
	hwnd = CreateWindow(
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
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
	g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// �̹��� �ε�
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
	// message ó��
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

	// input ó��
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
	// GDI+ ����
	delete* playerIdleFrames;
	delete* playerWalkFrames;
	delete* playerAttackFrames;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// �ܼ� ��� ����
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