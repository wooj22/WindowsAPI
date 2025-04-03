#pragma once

namespace Message {
	
	// 윈도우 프로시저
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	const char* GetMessageName(UINT msg);
	void PrintLastErrorMessage();
}