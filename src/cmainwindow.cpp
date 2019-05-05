#include "cmainwindow.h"

#define WND_CLASS_MAIN_NAME L"MyMainWindow"
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3


HWND CMainWindow::createHwnd(CWindow *self, int32_t width, int32_t height, const wchar_t *title, HWND parent)
{
    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW,
        WND_CLASS_NAME,
        title,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        parent,
        nullptr,
        GetModuleHandle(nullptr),
        reinterpret_cast<LPVOID>(self));

    if (hwnd == NULL) {
        return nullptr;
    }
    //TODO: center the window on screen
    return hwnd;
}

LRESULT CMainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugString(L"Creating main window...");
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

void CMainWindow::initializeMenus() {
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