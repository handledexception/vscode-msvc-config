#include <iostream>

#include "cwindow.h"
// #define IDM_FILE_NEW 1
// #define IDM_FILE_OPEN 2
// #define IDM_FILE_QUIT 3

CWindow::CWindow(int32_t width, int32_t height, const wchar_t *title, HWND parent)
:
mWidth(width),
mHeight(height),
mHwnd(nullptr)
{    
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

// HWND CWindow::createHwnd(CWindow *self, int32_t width, int32_t height, const wchar_t *title, HWND parent)
// {    
//     HWND hwnd = CreateWindowEx(
//         WS_EX_APPWINDOW,
//         WND_CLASS_NAME,
//         title,
//         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
//         CW_USEDEFAULT, CW_USEDEFAULT,
//         width, height,
//         parent,
//         nullptr,
//         GetModuleHandle(nullptr),
//         reinterpret_cast<LPVOID>(self));

//     if (hwnd == NULL) {
//         return nullptr;
//     }
//     //TODO: center the window on screen
//     return hwnd;
// }

// void CWindow::initializeMenus() {
//     HMENU hMenubar = ::CreateMenu();
//     HMENU hMenu = ::CreateMenu();

//     ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
//     ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
//     ::AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
//     ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

//     ::AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");
//     ::SetMenu(mHwnd, hMenubar);
//     ::DrawMenuBar(mHwnd);
// }