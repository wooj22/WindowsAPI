#include "pch.h"
#include <stdio.h>
#include "GDIRenderer.h"

namespace GDIRenderer {
	// Device Context
	HDC g_FrontBufferDC;
	HDC g_BackBufferDC;

	// Graphics
	Gdiplus::Graphics* g_pBackBufferGraphics;
	HBITMAP g_BackBufferBitmap;
	ULONG_PTR g_GdiPlusToken;


	/// GDIRenderer Initialize
	void Initalize(HWND hwnd, int width, int height) {
		// ���� ���۸� �ʱ�ȭ
		g_FrontBufferDC = GetDC(hwnd);
		g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
		g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, width, height);
		SelectObject(g_BackBufferDC, g_BackBufferBitmap);

		// GDI+ �ʱ�ȭ
		Gdiplus::GdiplusStartupInput gsi;
		Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
		g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);
	}

	/// Load Image
	void BitmapLoad(Gdiplus::Bitmap*& bitmap, wchar_t* filePath) {
		bitmap = new Gdiplus::Bitmap(filePath);
	}

	/// Square Paint (��׶����)
	void PaintingSquare(int width, int height) {
		PatBlt(g_BackBufferDC, 0, 0, width, height, BLACKNESS);
	}
	
	/// Image Draw
	void BitmapDraw(Gdiplus::Bitmap* bitmap, int posX, int posY) {
		GDIRenderer::g_pBackBufferGraphics->DrawImage(bitmap, posX, posY);
	}

	/// Front ���
	void DrawBackToFront(int width, int height) {
		BitBlt(g_FrontBufferDC, 0, 0, width, height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	/// GDIRenderer Release
	void Release(HWND hwnd) {
		// GDI+ ����
		delete g_pBackBufferGraphics;
		Gdiplus::GdiplusShutdown(g_GdiPlusToken);
		DeleteObject(g_BackBufferBitmap);
		DeleteDC(g_BackBufferDC);	
		ReleaseDC(hwnd, g_FrontBufferDC);
	}
}
