#include "pch.h"
#include <stdio.h>
#include "GDIRenderer.h"

namespace GDIRenderer {
	HDC g_FrontBufferDC;
	HDC g_BackBufferDC;
	Gdiplus::Graphics* g_pBackBufferGraphics;
	HBITMAP g_BackBufferBitmap;
	ULONG_PTR g_GdiPlusToken;

	/*----------------------- Animation  -------------------------*/
	/*--------------------- // TODO : 분리 -----------------------*/
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

	/// Player Idle Animation
	inline void PlayerIdleAnimation(int g_width, int g_height) {
		if (animationIndex > IDLE_SIZE - 1) animationIndex = 0;

		GDIRenderer::g_pBackBufferGraphics->DrawImage(playerIdleFrames[animationIndex],
			g_width / 2 - (int)playerIdleFrames[animationIndex]->GetWidth() / 2,
			g_height / 2 - (int)playerIdleFrames[animationIndex]->GetHeight() / 2);

		prePlayerState = playerState;
		printf("IDLE - currentIndex [%d]\n", animationIndex);
	}

	/// Player Walk Animation
	inline void PlayerWalkAnimation(int g_width, int g_height) {
		if (animationIndex > WALK_SIZE - 1)	animationIndex = 0;

		GDIRenderer::g_pBackBufferGraphics->DrawImage(playerWalkFrames[animationIndex],
			g_width / 2 - (int)playerWalkFrames[animationIndex]->GetWidth() / 2,
			g_height / 2 - (int)playerWalkFrames[animationIndex]->GetHeight() / 2);

		prePlayerState = playerState;
		printf("WALK - currentIndex [%d]\n", animationIndex);
	}

	/// Player Attack Animation
	inline void PlayerAttackAnimation(int g_width, int g_height) {
		GDIRenderer::g_pBackBufferGraphics->DrawImage(playerAttackFrames[animationIndex],
			g_width / 2 - (int)playerAttackFrames[animationIndex]->GetWidth() / 2,
			g_height / 2 - (int)playerAttackFrames[animationIndex]->GetHeight() / 2);

		prePlayerState = playerState;
		printf("ATTACK - currentIndex [%d]\n", animationIndex);

		// 1회 재생 후 IDLE로 전환
		if (animationIndex == ATTACK_SIZE - 1) playerState = IDLE;
	}
	/*---------------------------------------------------------------------*/
	/*---------------------------------------------------------------------*/


	/// Renderer Initialize
	void Initalize(HWND hwnd, int g_width, int g_height) {
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

		// Animation 초기화
		animationTimer = 0.0f;
		animationIndex = 0;
	}

	/// Renderer Update
	void Update() {
		animationTimer += Time::GetDeltaTime();

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

	/// Renderer Render
	void Render(int g_width, int g_height) {
		PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);
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
		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	/// Renderer Release
	void Release(HWND hwnd) {
		// GDI+ 해제
		delete* playerIdleFrames;
		delete* playerWalkFrames;
		delete* playerAttackFrames;
		delete g_pBackBufferGraphics;

		Gdiplus::GdiplusShutdown(g_GdiPlusToken);
		DeleteObject(g_BackBufferBitmap);
		DeleteDC(g_BackBufferDC);	
		ReleaseDC(hwnd, g_FrontBufferDC);
	}
}
