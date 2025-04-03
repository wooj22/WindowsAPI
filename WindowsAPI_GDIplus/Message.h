#pragma once

namespace Message {
	
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	const char* GetMessageName(UINT msg);
	void PrintLastErrorMessage();
}