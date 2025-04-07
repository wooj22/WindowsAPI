#pragma once
#include "gdiplus.h"
#pragma comment(lib, "gdiplus.lib")

namespace GDIRenderer {

	void Initalize(HWND hwnd, int width, int height);
	void BitmapLoad(Gdiplus::Bitmap* bitmap, wchar_t* filePath);

	void Update();

	void PaintingSquare(int width, int height);
	void DrawBackToFront(int width, int height);
	void Draw(Gdiplus::Bitmap* bitmap, int posX, int posY);
	void Render(int width, int height);

	void Release(HWND hwnd);
}