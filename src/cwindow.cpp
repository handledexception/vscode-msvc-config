#include <iostream>

#include "cwindow.h"

#define WND_CLASS_NAME L"My Application"
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3

CWindow::CWindow(int32_t width, int32_t height, const wchar_t *title)
:
mWidth(width),
mHeight(height),
mHwnd(nullptr)
{
    mHwnd = createHwnd(this, width, height, title);

    SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

CWindow::~CWindow()
{
    if (mHwnd) {
        DestroyWindow(mHwnd);
    }
}

void CWindow::Show(bool show)
{
    if (show == true) {
        ShowWindowAsync(mHwnd, SW_SHOWNORMAL);
    }
    else {
        ShowWindowAsync(mHwnd, SW_HIDE);
    }

    UpdateWindow(mHwnd);
}

HWND CWindow::createHwnd(CWindow *self, int32_t width, int32_t height, const wchar_t *title)
{
    OutputDebugString(L"Creating window...\n");
    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        WND_CLASS_NAME,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        reinterpret_cast<LPVOID>(self));

    if (hwnd == NULL) {
        return nullptr;
    }
    //TODO: center the window on screen
    return hwnd;
}

void CWindow::initializeMenus() {
    HMENU hMenubar = ::CreateMenu();
    HMENU hMenu = ::CreateMenu();

    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
    ::AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

    ::AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");
    ::SetMenu(mHwnd, hMenubar);
    ::DrawMenuBar(mHwnd);
}

LRESULT CWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugString(L"Creating window...");
            initializeMenus();
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam)) {
                case IDM_FILE_QUIT:
                    PostMessage(mHwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(mHwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(mHwnd, msg, wparam, lparam);
}