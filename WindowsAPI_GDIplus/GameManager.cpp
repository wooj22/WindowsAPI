#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "GameManager.h"
#include "Message.h"
#include "Time.h"
#include "Input.h"
#pragma comment(lib, "gdiplus.lib")

/*----------------------- global -------------------------*/
// Window Data
LPCTSTR g_szClassName = TEXT("������ Ŭ���� �̸�");
int g_width = 1024;
int g_height = 768;

// Window Handle
HWND g_hWnd;

// HDC (Device Context) 
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// GDI+ �׷��� ��ü
Gdiplus::Graphics* g_pBackBufferGraphics;

// BITMAP
HBITMAP g_BackBufferBitmap;

// Player
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

Gdiplus::Bitmap* playerIdle[IDLE_SIZE];
Gdiplus::Bitmap* playerWalk[WALK_SIZE];
Gdiplus::Bitmap* playerAttack[ATTACK_SIZE];
float animationTimer = 0.0f;
float animationCycle = 0.17f;
int animationIndex = 0;


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

/// �ִϸ��̼�
void PlayerIdleAnimation() {
	if (animationIndex > IDLE_SIZE-1 || playerState != prePlayerState) 
		animationIndex = 0;

	g_pBackBufferGraphics->DrawImage(playerIdle[animationIndex],
		g_width / 2 - (int)playerIdle[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerIdle[animationIndex]->GetHeight() / 2);

	printf("IDLE - currentIndex [%d]\n", animationIndex);
	prePlayerState = playerState;

	animationIndex++;
}

void PlayerWalkAnimation() {
	if (animationIndex > WALK_SIZE-1 ||playerState != prePlayerState)
		animationIndex = 0;
	
	g_pBackBufferGraphics->DrawImage(playerWalk[animationIndex],
		g_width / 2 - (int)playerWalk[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerWalk[animationIndex]->GetHeight() / 2);

	printf("WALK - currentIndex [%d]\n", animationIndex);
	prePlayerState = playerState;

	animationIndex++;
}

void PlayerAttackAnimation() {
	if (animationIndex > ATTACK_SIZE-1 || playerState != prePlayerState)
		animationIndex = 0;
	
	g_pBackBufferGraphics->DrawImage(playerAttack[animationIndex],
		g_width / 2 - (int)playerAttack[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerAttack[animationIndex]->GetHeight() / 2);

	printf("ATTACK - currentIndex [%d]\n", animationIndex);
	prePlayerState = playerState;

	animationIndex++;
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
	g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// �̹��� �ε�
	wchar_t filePath[256];

	// Idle
	for (int i = 0; i < IDLE_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/idle-%d.png", i + 1);
		playerIdle[i] = new Gdiplus::Bitmap(filePath);
	}

	// Walk
	for (int i = 0; i < WALK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/walk-%d.png", i + 1);
		playerWalk[i] = new Gdiplus::Bitmap(filePath);
	}

	// Attack
	for (int i = 0; i < ATTACK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/attack-A%d.png", i + 1);
		playerAttack[i] = new Gdiplus::Bitmap(filePath);
	}

	// NonBlocking����� Game Loop
	MSG msg;

	// ���� �� Ÿ�̸� �ʱ�ȭ
	Time::Initialize();
	animationTimer = 0.0f;
	animationIndex = 0;
	
	while (true)
	{
		// updtate
		Time::UpdateTime();
		animationTimer += Time::GetDeltaTime();
		Input::Update();

		// message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// input
		if (Input::IsKeyPressed('1')) {
			prePlayerState = playerState;
			playerState = IDLE;
		}
		if (Input::IsKeyPressed('2')) {
			prePlayerState = playerState;
			playerState = WALK;
		}
		if (Input::IsKeyPressed('3')) {
			prePlayerState = playerState;
			playerState = ATTACK;
		}

		// render
		if (animationTimer >= animationCycle) {
			PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);
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
			animationTimer = 0.0f;
		}

		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	// GDI+ ����
	delete* playerIdle;
	delete* playerWalk;
	delete* playerAttack;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// �ܼ� ��� ����
	UninitConsole();
	return (int)msg.wParam;
}