//#include <stdio.h>
//#include <windows.h>
//#include <gdiplus.h>
//#pragma comment(lib, "gdiplus.lib")
//
//
//LPCTSTR g_szClassName = TEXT("윈도우 클래스 이름");
//int g_width = 1024;
//int g_height = 768;
//
//// HDC (Device Context) 
//// Device Context (HDC) : GDI가 그림을 그릴 때 사용하는 출력 대상 및 상태 정보를 담은 객체
//HWND g_hWnd;
//HDC g_FrontBufferDC;    // 앞면
//HDC g_BackBufferDC;     // 뒷면
//
//// HBITMAP
//HBITMAP g_BackBufferBitmap;
//
//
///*----------------------- Funtions -------------------------*/
///// 콘솔창 생성
//void InitConsole()
//{
//	AllocConsole();
//	FILE* fp;
//	freopen_s(&fp, "CONOUT$", "w", stdout);
//	SetConsoleTitle(L"윈도우 메시지 콘솔 로그");
//	printf("콘솔 로그 시작...\n\n");
//}
//
///// 콘솔창 해제
//void UninitConsole()
//{
//	fclose(stdout);
//	FreeConsole();
//}
//
///// 마지막으로 발생한 WIN32 API 에러의 메시지를 출력
//void PrintLastErrorMessage()
//{
//	// 최근 발생한 오류 코드
//	DWORD errorCode = GetLastError();
//	LPVOID lpMsgBuf;
//
//	// 오류 코드에 해당하는 시스템 메시지
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL,
//		errorCode,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPSTR)&lpMsgBuf,
//		0,
//		NULL);
//
//	if (lpMsgBuf)
//	{
//		printf("오류 코드: %lu\n오류 메시지: %s\n", errorCode, (char*)lpMsgBuf);
//		LocalFree(lpMsgBuf); // FormatMessageA에서 할당된 버퍼 해제
//	}
//	else
//	{
//		printf("오류 코드: %lu (메시지를 찾을 수 없음)\n", errorCode);
//	}
//}
//
///*
//윈도우 프로시저 메시지 콜백
//@ LRESULT : 메시지 처리 결과를 반환하는 타입(정수형 값)
//@ CALLBACK : 호출 규약을 나타내는 키워드(윈도우 API의 표준 호출 방식)
//@ HWND hwnd : 현재 메시지를 받은 창의 핸들
//@ UINT msg : 어떤 메시지인지 식별하는 값(예: WM_DESTROY, WM_PAINT 등)
//@ WPARAM wParam, LPARAM lParam  : 추가적인 메시지 정보를 포함하는 매개변수
//*/
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	}
//
//	// 기본 창 프로시저로 메시지 전달
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
//
///*-------------------------- Main ----------------------------*/
///*
//WinMain : Windows 애플리케이션의 진입점
//@ hInstance : 현재 실행 중인 프로그램의 인스턴스 핸들
//@ hPrevInstance : 과거 버전의 인스턴스 핸들 (현재는 항상 NULL)
//@ lpCmdLine : 명령줄 인수
//@ nCmdShow : 창을 어떻게 표시할지 결정하는 값 (SW_SHOW, SW_HIDE 등)
//*/
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	// 콘솔창 생성
//	InitConsole();
//
//	// 현재 디렉토리
//	char szPath[MAX_PATH] = { 0, };
//	::GetCurrentDirectoryA(MAX_PATH, szPath);
//	//printf("Current Directory: %s\n", szPath);
//
//	// WNDCLASS 정의
//	WNDCLASS wc = { 0 };
//	wc.lpfnWndProc = WndProc;
//	wc.hInstance = hInstance;
//	wc.lpszClassName = g_szClassName;
//	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//
//	// RegisterClass : WNDCLASS 등록
//	RegisterClass(&wc);
//
//	// AdjustWindowRect :  rcClient를 WS_OVERLAPPEDWINDOW(표준 윈도우 스타일)에 맞춰서 창 크기 조정
//	RECT rcClient = { 0, 0, (LONG)g_width, (LONG)g_height };
//	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);
//
//	// CreateWindow : 윈도우 생성
//	HWND hwnd = CreateWindow(
//		g_szClassName,
//		TEXT("GDL plus 애니메이션 과제중"),
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT, CW_USEDEFAULT,		// 창의 초기 위치
//		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,	// 창 크기(x, y)
//		NULL, NULL, hInstance, NULL);
//
//
//	// ShowWindow : 윈도우를 화면에 표시
//	ShowWindow(hwnd, nCmdShow);
//
//	// UpdateWindow : WM_PAINT 이벤트 생성(윈도우 강제 갱신)
//	UpdateWindow(hwnd);
//
//	// 더블 버퍼링을 위한 초기화
//	g_FrontBufferDC = GetDC(hwnd);
//	g_BackBufferDC = CreateCompatibleDC(g_FrontBufferDC);
//	g_BackBufferBitmap = CreateCompatibleBitmap(g_FrontBufferDC, g_width, g_height);
//	SelectObject(g_BackBufferDC, g_BackBufferBitmap);     // 백 버퍼 Device Context에 비트맵 연결
//
//	// GDI+ 초기화
//	// GdiplusStartup(GDI+ 토큰, 초기화 설정, 추가 디버깅 정보)
//	// Graphics클래스 : GDI+를 사용하여 2D 그래픽을 그리는 역할
//	ULONG_PTR g_GdiPlusToken;			// GDI+의 상태를 관리하는 식별자(핸들)
//	Gdiplus::GdiplusStartupInput gsi;	// GDI+의 설정 값을 담고있는 구조체
//	Gdiplus::GdiplusStartup(&g_GdiPlusToken, &gsi, nullptr);
//
//	// g_BackBufferDC를 기반으로 하는 GDI+ 그래픽 객체 생성
//	Gdiplus::Graphics* g_pBackBufferGraphics = Gdiplus::Graphics::FromHDC(g_BackBufferDC);
//
//	// 이미지 로드
//	// Bitmap 클래스 : PNG, JPG 등의 이미지 파일을 로드하고 비트맵 형태로 저장
//	Gdiplus::Bitmap* g_pImageBitmap = new Gdiplus::Bitmap(L"../Resource/PlayerAnimation/attack-A1.png");
//	UINT witdh = g_pImageBitmap->GetWidth();
//	UINT height = g_pImageBitmap->GetHeight();
//
//	if (g_pImageBitmap->GetLastStatus() != Gdiplus::Ok)
//	{
//		MessageBox(hwnd, L"PNG 파일 로드 실패", L"오류", MB_ICONERROR);
//		PostQuitMessage(0);
//	}
//
//	MSG msg;
//
//	// Renderer Loop
//	while (true)
//	{
//		// PeekMessage : 메시지 큐에서 메시지 get
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.message == WM_QUIT)
//				break;
//
//			TranslateMessage(&msg);	// 키보드 입력 메시지
//			DispatchMessage(&msg);	// 메시지를 윈도우 프로시저로 전달
//		}
//
//		// Renderer::BeginDraw()
//		// PatBlt(대상 hdc, posX, posY, width,  height, 래스터 연산 코드)
//		// 특정 영역을 채우는 함수 (background)
//		PatBlt(g_BackBufferDC, 0, 0, g_width, g_height, BLACKNESS);
//
//		// Render()
//		// DrawImage(Image*. posX, posY) : image객체의 왼쪽상단 기준으므로 게임 그래픽 렌더시에 축 보정 필요
//		g_pBackBufferGraphics->DrawImage(g_pImageBitmap,
//			g_width / 2 - (int)g_pImageBitmap->GetWidth() / 2, g_height / 2 - (int)g_pImageBitmap->GetHeight() / 2);
//
//		// Renderer::EndDraw()
//		// BitBlt(대상 hdc, posX, posY, witdh, height, 원본 hdc, 원본 posX, 원본 posY, 래스터연산코드)
//		// 한 Device Context(DC)에서 다른 DC로 이미지 데이터를 빠르게 복사하는 함수
//		// 백 버퍼에 그려진 내용을 화면에 출력하는 데 사용
//		BitBlt(g_FrontBufferDC, 0, 0, g_width, g_height, g_BackBufferDC, 0, 0, SRCCOPY);
//	}
//
//	// Renderer::Uninitialize
//	// GDI+ 해제
//	delete g_pImageBitmap;
//	delete g_pBackBufferGraphics;
//	Gdiplus::GdiplusShutdown(g_GdiPlusToken);
//
//	DeleteObject(g_BackBufferBitmap);
//	DeleteDC(g_BackBufferDC);
//	ReleaseDC(hwnd, g_FrontBufferDC);
//
//	UninitConsole();  // 콘솔 출력 해제
//	return (int)msg.wParam;
//}
