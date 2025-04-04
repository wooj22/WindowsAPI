#pragma once
#include "gdiplus.h"
#pragma comment(lib, "gdiplus.lib")

namespace GDIRenderer {
	void Initalize(HWND hwnd, int g_width, int g_height);
	void Update();
	void Render(int g_width, int g_height);
	void Release(HWND hwnd);
}