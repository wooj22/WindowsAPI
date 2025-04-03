#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")


LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");

// Window Size
int g_width = 1024;
int g_height = 768;

// HDC (Device Context) 
HWND g_hWnd;
HDC g_FrontBufferDC;
HDC g_BackBufferDC;

// BITMAP
HBITMAP g_BackBufferBitmap;


/*----------------------- Funtions -------------------------*/
/// 콘솔창 생성
void InitConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"윈도우 메시지 콘솔 로그");
	printf("콘솔 로그 시작...\n\n");
}

/// 콘솔창 해제
void UninitConsole()
{
	fclose(stdout);
	FreeConsole();
}

/// 마지막으로 발생한 WIN32 API 에러의 메시지를 출력
void PrintLastErrorMessage()
{
	// 최근 발생한 오류 코드
	DWORD errorCode = GetLastError();
	LPVOID lpMsgBuf;

	// 오류 코드에 해당하는 시스템 메시지
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL);

	if (lpMsgBuf)
	{
		printf("오류 코드: %lu\n오류 메시지: %s\n", errorCode, (char*)lpMsgBuf);
		LocalFree(lpMsgBuf); // FormatMessageA에서 할당된 버퍼 해제
	}
	else
	{
		printf("오류 코드: %lu (메시지를 찾을 수 없음)\n", errorCode);
	}
}

// 메시지
const char* GetMessageName(UINT msg)
{
	switch (msg)
	{
	case WM_NULL: return "WM_NULL";
	case WM_CREATE: return "WM_CREATE";
	case WM_DESTROY: return "WM_DESTROY";
	case WM_MOVE: return "WM_MOVE";
	case WM_SIZE: return "WM_SIZE";
	case WM_ACTIVATE: return "WM_ACTIVATE";
	case WM_SETFOCUS: return "WM_SETFOCUS";
	case WM_KILLFOCUS: return "WM_KILLFOCUS";
	case WM_ENABLE: return "WM_ENABLE";
	case WM_SETREDRAW: return "WM_SETREDRAW";
	case WM_SETTEXT: return "WM_SETTEXT";
	case WM_GETTEXT: return "WM_GETTEXT";
	case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
	case WM_PAINT: return "WM_PAINT";
	case WM_CLOSE: return "WM_CLOSE";
	case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
	case WM_QUIT: return "WM_QUIT";
	case WM_QUERYOPEN: return "WM_QUERYOPEN";
	case WM_ERASEBKGND: return "WM_ERASEBKGND";
	case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
	case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
	case WM_WININICHANGE: return "WM_WININICHANGE";
	case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
	case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
	case WM_FONTCHANGE: return "WM_FONTCHANGE";
	case WM_TIMECHANGE: return "WM_TIMECHANGE";
	case WM_CANCELMODE: return "WM_CANCELMODE";
	case WM_SETCURSOR: return "WM_SETCURSOR";
	case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
	case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
	case WM_QUEUESYNC: return "WM_QUEUESYNC";
	case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";

		// 마우스 관련	
	case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
	case WM_LBUTTONUP: return "WM_LBUTTONUP";
	case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
	case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
	case WM_RBUTTONUP: return "WM_RBUTTONUP";
	case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
	case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
	case WM_MBUTTONUP: return "WM_MBUTTONUP";
	case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
	case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
	case WM_MOUSEHWHEEL: return "WM_MOUSEHWHEEL";

		// 키보드 관련
	case WM_KEYDOWN: return "WM_KEYDOWN";
	case WM_KEYUP: return "WM_KEYUP";
	case WM_CHAR: return "WM_CHAR";
	case WM_DEADCHAR: return "WM_DEADCHAR";
	case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
	case WM_SYSKEYUP: return "WM_SYSKEYUP";
	case WM_SYSCHAR: return "WM_SYSCHAR";
	case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
	case WM_HOTKEY: return "WM_HOTKEY";

		// 비클라이언트 영역
	case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
	case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
	case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
	case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
	case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
	case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
	case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
	case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
	case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
	case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
	case WM_NCHITTEST: return "WM_NCHITTEST";
	case WM_NCPAINT: return "WM_NCPAINT";
	case WM_NCACTIVATE: return "WM_NCACTIVATE";

		// 시스템 명령
	case WM_SYSCOMMAND: return "WM_SYSCOMMAND";

		// 기타
	case WM_TIMER: return "WM_TIMER";
	case WM_COMMAND: return "WM_COMMAND";
	case WM_INITDIALOG: return "WM_INITDIALOG";

	default: return "Unknown Message";
	}
}

/// 윈도우 프로시저 메시지 콜백
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		printf("WM_CREATE\n");
		break;
	case WM_PAINT:
		printf("WM_PAINT\n");
		break;
	case WM_LBUTTONDOWN:
		printf("WM_LBUTTONDOWN : 클릭 위치 x=%d y=%d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		printf("WM_LBUTTONUP : 클릭 위치 x=%d y=%d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		printf("WM_RBUTTONDOWN : 클릭 위치 x=%d y=%d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		printf("WM_RBUTTONUP : 클릭 위치 x=%d y=%d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		printf("WM_KEYDOWN: VK_CODE = %d\n", (int)wParam);
		break;
	case WM_KEYUP:
		printf("WM_KEYUP: VK_CODE = %d\n", (int)wParam);
		break;
	case WM_CHAR:
		printf("WM_CHAR: 문자 입력 = '%c'\n", (char)wParam);
		break;
	case WM_MOUSEMOVE:
	case WM_NCHITTEST:
	case WM_NCMOUSEMOVE:
	case WM_SETCURSOR:
		break;
	case WM_DESTROY:
		printf("WM_DESTROY: 프로그램 종료\n");
		PostQuitMessage(0);
		break;
	default:
		printf("%s (0x%04X)\n", GetMessageName(msg), msg);
		break;
	}

	// 기본 창 프로시저로 메시지 전달
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*-------------------------- Main ----------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 콘솔창 생성
	InitConsole();

	// 디렉토리 경로 저장
	char szPath[MAX_PATH] = { 0, };
	::GetCurrentDirectoryA(MAX_PATH, szPath);

	// WNDCLASS 정의
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = g_szClassName;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	// RegisterClass : WNDCLASS 등록
	RegisterClass(&wc);

	// 창 크기 조절
	RECT rcClient = { 0, 0, (LONG)g_width, (LONG)g_height };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

	// 윈도우 생성
	HWND hwnd = CreateWindow(
		g_szClassName,
		TEXT("GDL plus 애니메이션 과제중"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		NULL, NULL, hInstance, NULL);


	// Show
	ShowWindow(hwnd, nCmdShow);

	// WM_PAINT 이벤트 생성
	UpdateWindow(hwnd);

	// 더블 버퍼링 초기화
	g_FrontBufferDC = GetDC(hwnd);
	g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
	g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, g_width, g_height);
	SelectObject(g_BackBufferDC, g_BackBufferBitmap);

	// GDI+ 초기화
	ULONG_PTR g_GdiPlusToken;
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);

	// g_BackBufferDC와 호환되는 그래픽 객체 생성
	Gdiplus::Graphics* g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);

	// 이미지 로드
	Gdiplus::Bitmap* g_pImageBitmap = new Gdiplus::Bitmap(L"../Resource/PlayerAnimation/attack-A1.png");
	UINT witdh = g_pImageBitmap->GetWidth();
	UINT height = g_pImageBitmap->GetHeight();

	if (g_pImageBitmap->GetLastStatus() != Gdiplus::Ok)
	{
		MessageBox(hwnd, L"PNG 파일 로드 실패", L"오류", MB_ICONERROR);
		PostQuitMessage(0);
	}

	// NonBlocking방식의 Game Loop
	MSG msg;
	while (true)
	{
		// 메시지 get -> 윈도우 프로시저 전달
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 채우기
		PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);

		// 그리기
		g_pBackBufferGraphics->DrawImage(g_pImageBitmap,
			g_width / 2 - (int)g_pImageBitmap->GetWidth() / 2, g_height / 2 - (int)g_pImageBitmap->GetHeight() / 2);

		// 복사 출력하기
		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
	}

	// GDI+ 해제
	delete g_pImageBitmap;
	delete g_pBackBufferGraphics;
	Gdiplus::GdiplusShutdown(g_GdiPlusToken);

	DeleteObject(g_BackBufferBitmap);
	DeleteDC(g_BackBufferDC);
	ReleaseDC(hwnd, g_FrontBufferDC);

	// 콘솔 출력 해제
	UninitConsole();  
	return (int)msg.wParam;
}
