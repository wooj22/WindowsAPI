#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include "GameManager.h"
#include "Message.h"
#include "../Module/Time.h"
#include "../Module/Input.h"
#include "../Module/ConsoleRenderer.h"
#include "../Module/GDIRenderer.h"

/*----------------------- global data  -------------------------*/
// Window
LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");
HWND hwnd;	   		
MSG msg;			
int g_width = 1024;
int g_height = 768;

// Character Animation State
enum CharacterState
{
	IDLE,
	WALK,
	ATTACK
};

CharacterState playerState = IDLE;
CharacterState prePlayerState = IDLE;

// Character Animation Frames
const int IDLE_SIZE = 4;
const int WALK_SIZE = 6;
const int ATTACK_SIZE = 8;

Gdiplus::Bitmap* playerIdleFrames[IDLE_SIZE];
Gdiplus::Bitmap* playerWalkFrames[WALK_SIZE];
Gdiplus::Bitmap* playerAttackFrames[ATTACK_SIZE];

// Character Animation Cycle & Timer
float animationTimer = 0.0f;
float animationCycle = 0.17f;
int animationIndex = 0;

// Loop Flag
bool g_Loop = true;



/*----------------------- Console  -------------------------*/
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



/*----------------------- Window  -------------------------*/
inline void WindowCreating(HINSTANCE hInstance, int nCmdShow) {
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
}




/*----------------------- Image Load  -------------------------*/
/// Animation Frames Load
inline void LoadAnimationFrames() 
{
	wchar_t filePath[256];

	// Idle
	for (int i = 0; i < IDLE_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/idle-%d.png", i + 1);
		GDIRenderer::BitmapLoad(playerIdleFrames[i], filePath);
	}

	// Walk
	for (int i = 0; i < WALK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/walk-%d.png", i + 1);
		GDIRenderer::BitmapLoad(playerWalkFrames[i], filePath);
	}

	// Attack
	for (int i = 0; i < ATTACK_SIZE; ++i) {
		swprintf_s(filePath, L"../Resource/PlayerAnimation/attack-A%d.png", i + 1);
		GDIRenderer::BitmapLoad(playerAttackFrames[i], filePath);
	}
}



/*----------------------- Animation  -------------------------*/
/// Player Idle Animation
inline void PlayerIdleAnimation(int g_width, int g_height) 
{
	if (animationIndex > IDLE_SIZE - 1) animationIndex = 0;

	GDIRenderer::BitmapDraw(playerIdleFrames[animationIndex],
		g_width / 2 - (int)playerWalkFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerWalkFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;
}


/// Player Walk Animation
inline void PlayerWalkAnimation(int g_width, int g_height) 
{
	if (animationIndex > WALK_SIZE - 1)	animationIndex = 0;

	GDIRenderer::BitmapDraw(playerWalkFrames[animationIndex],
		g_width / 2 - (int)playerWalkFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerWalkFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;
}


/// Player Attack Animation
inline void PlayerAttackAnimation(int g_width, int g_height) 
{
	GDIRenderer::BitmapDraw(playerAttackFrames[animationIndex],
		g_width / 2 - (int)playerWalkFrames[animationIndex]->GetWidth() / 2,
		g_height / 2 - (int)playerWalkFrames[animationIndex]->GetHeight() / 2);

	prePlayerState = playerState;

	// 1회 재생 후 IDLE로 전환
	if (animationIndex == ATTACK_SIZE - 1) playerState = IDLE;
}



/*------------------------ Gamemanager -------------------------------*/
namespace GameManager {

	/// Initalize
	inline void Initalize(HINSTANCE hInstance, int nCmdShow) 
	{
		InitConsole();						 // 콘솔창 생성
		WindowCreating(hInstance, nCmdShow); // 윈도우 생성

		GDIRenderer::Initalize(hwnd, g_width, g_height); // GDI 모듈 초기화
		LoadAnimationFrames();			                 // 애니메이션 프레임 로드
		
		// Timer 초기화
		Time::Initialize();
		animationTimer = 0.0f;
		animationIndex = 0;		// 애니메이션 프레임 초기화
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

		// time update
		Time::UpdateTime();
		Input::Update();
		animationTimer += Time::GetDeltaTime();
	}

	/// Render
	inline void Render() {
		GDIRenderer::PaintingSquare(g_width, g_height);
		
		// Animation Draw
		switch (playerState)
		{
		case IDLE:
			PlayerIdleAnimation(g_width, g_height);
			break;
		case WALK:
			PlayerWalkAnimation(g_width, g_height);
			break;
		case ATTACK:
			PlayerAttackAnimation(g_width, g_height);
			break;
		default:
			break;
		}

		GDIRenderer::DrawBackToFront(g_width, g_height);
	}

	/// Clear
	inline void Release() {
		delete* playerIdleFrames;
		delete* playerWalkFrames;
		delete* playerAttackFrames;

		GDIRenderer::Release(hwnd);
		UninitConsole();
	}
}

/*-------------------------- Main Program ----------------------------*/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow)
{
	GameManager::Initalize(hInstance, nCmdShow);

	while (g_Loop)
	{
		GameManager::Update();
		GameManager::Render();
	}

	GameManager::Release();
	return (int)msg.wParam;
}