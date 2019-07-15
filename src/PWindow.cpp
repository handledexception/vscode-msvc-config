#include "PWindow.h"

#define MENU_FILE_NEW 1
#define MENU_FILE_OPEN 2
#define MENU_FILE_QUIT 3

PWindow::PWindow(
	wchar_t* title,
	uint32_t cx,
	uint32_t cy,
	uint32_t width,
	uint32_t height,
	HINSTANCE hinst,
	HWND parent
)
:
mWindowTitle(title),
mWindowClassName(nullptr),
mPosX(cx),
mPosY(cy),
mWidth(width),
mHeight(height),
mInstance(hinst),
mParentHwnd(parent),
mHwnd(nullptr)
{
}

PWindow::~PWindow() {
	if (mHwnd) {
		DestroyWindow(mHwnd);
		mHwnd = nullptr;
	}
}

bool PWindow::Init(int32_t hwndExStyle, int32_t hwndStyle) {
	register_class();
	if (!create_hwnd(hwndExStyle, hwndStyle))
		return false;

	return true;
}

LRESULT PWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_CREATE) {
		OutputDebugString(L"PWindow::WndProc -> WM_CREATE\n");
	}

	if (msg == WM_COMMAND) {
		OutputDebugString(L"Pwindow::WndProc -> WM_COMMAND");
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void PWindow::Show(bool show) {
	if (show == true) {
		ShowWindowAsync(mHwnd, SW_SHOWNORMAL);
	}
	else {
		ShowWindowAsync(mHwnd, SW_HIDE);
	}

	UpdateWindow(mHwnd);
}

LRESULT CALLBACK PWindow::static_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_CREATE) {
		PWindow* wnd = reinterpret_cast<PWindow*>(
			reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams
		);
		if (wnd && wnd->GetHandle() == nullptr) {
			wnd->SetHandle(hwnd);
		}

		return wnd->WndProc(hwnd, msg, wparam, lparam);
	}

	if (msg == WM_CLOSE) {
		OutputDebugString(L"PWindow::static_wndproc -> WM_CLOSE\n");
	}

	PWindow* wnd = reinterpret_cast<PWindow*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (wnd) {
		wnd->SetHandle(hwnd);
		return wnd->WndProc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void PWindow::register_class() {
	WNDCLASSEX wcex;

	// window class already registered
	if (GetClassInfoEx(mInstance, mWindowTitle, &wcex)) {
		OutputDebugString(L"Window class already registered!\n");
		return;
	}

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &PWindow::static_wndproc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = mWindowTitle;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (RegisterClassEx(&wcex) == 0) {
		OutputDebugString(L"Error registering window class!\n");
		return;
	}
}

HWND PWindow::create_hwnd(int32_t exStyle, int32_t style) {
	HWND hwnd = CreateWindowEx(
		exStyle,
		mWindowTitle,
		mWindowTitle,
		style,
		mPosX, mPosY,
		mWidth, mHeight,
		mParentHwnd,
		nullptr,
		mInstance,
		reinterpret_cast<LPVOID>(this));

	mHwnd = hwnd;

	SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	return hwnd;
}