#include "Graphics.h"

char szClassName[] = "gpics";

void (*frame)();
void (*click)(int x, int y);
int w, h, full, size;
HBITMAP bmp;
HDC dc, backdc;
HWND hwnd;

void resizeWindow(int width, int height) {
	if (full) return;
	if (width && height) {
		w = width;
		h = height;
		size = w*h*3;
		RECT wnd = {0, 0, w, h};
		AdjustWindowRect(&wnd, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, wnd.right-wnd.left, wnd.bottom-wnd.top, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOZORDER);
		DeleteObject(bmp); //ew
	}
	BITMAPINFO bmin;
	bmin.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmin.bmiHeader.biWidth = w;
	bmin.bmiHeader.biHeight = h;
	bmin.bmiHeader.biPlanes = 1;
	bmin.bmiHeader.biBitCount = 24;
	bmin.bmiHeader.biCompression = BI_RGB;
	bmin.bmiHeader.biSizeImage =
	bmin.bmiHeader.biXPelsPerMeter =
	bmin.bmiHeader.biYPelsPerMeter =
	bmin.bmiHeader.biClrUsed =
	bmin.bmiHeader.biClrImportant = 0;
	bmp = CreateDIBSection(dc, &bmin, DIB_RGB_COLORS, (void**) &pixels, NULL, 0);
	SelectObject(backdc, bmp);
}

void fillscreen() {
	DEVMODE dm;
	dm.dmSize = sizeof dm;
	dm.dmPelsWidth = w;
	dm.dmPelsHeight = h;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
		MessageBox(NULL, "Fullscreen mode failed", "Error", MB_OK);
		full = 0;
	}
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT paint;
	HDC localdc;
	switch (message) {
		case WM_CREATE:
			dc = GetDC(hwnd);
			backdc = CreateCompatibleDC(dc);
			resizeWindow(0, 0);
			srand(time(0));
			break;
		case WM_PAINT:
			localdc = BeginPaint(hwnd, &paint);
			BitBlt(localdc, 0, 0, w, h, backdc, 0, 0, SRCCOPY);
			EndPaint(hwnd, &paint);
			break;
		case WM_LBUTTONUP:
			if (click) (*click)(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_SETFOCUS:
			if (full) fillscreen();
			break;
		case WM_KILLFOCUS:
			if (full) {
				ChangeDisplaySettings(NULL, 0);
				ShowWindow(hwnd, SW_MINIMIZE);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int spawnWindow(HINSTANCE instance, int cmdShow, int width, int height, int fullscreen, const char* title, void (*onframe)(), void (*onclick)(int x, int y)) {
	w = width;
	h = height;
	full = fullscreen;
	frame = onframe;
	click = onclick;
	size = w*h*3;
	WNDCLASSEX wincl;
	wincl.hInstance = instance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.hIcon =
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = GetStockObject(WHITE_BRUSH); //(HBRUSH) COLOR_BACKGROUND;
	if (!RegisterClassEx(&wincl)) return 0;
	RECT wnd = {0, 0, w, h};
	if (full) fillscreen();
	if (full) {
		AdjustWindowRect(&wnd, WS_POPUP, FALSE);
		hwnd = CreateWindowEx(WS_EX_TOPMOST, szClassName, title, WS_POPUP,
			wnd.left, wnd.top, wnd.right-wnd.left, wnd.bottom-wnd.top, HWND_DESKTOP, NULL, instance, NULL);
	} else {
		AdjustWindowRect(&wnd, WS_OVERLAPPEDWINDOW, FALSE);
		hwnd = CreateWindowEx(0, szClassName, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			wnd.right-wnd.left, wnd.bottom-wnd.top, HWND_DESKTOP, NULL, instance, NULL); //but the client area!
	}
	ShowWindow(hwnd, cmdShow);
	BOOL quit = FALSE;
	MSG msg;
	while (!quit)
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) quit = TRUE;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			(*frame)();
			InvalidateRgn(hwnd, NULL, FALSE);
		}
	DeleteObject(backdc);
	DeleteObject(bmp);
	return msg.wParam;
}
