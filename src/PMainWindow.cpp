#include "PMainWindow.h"

#define WND_CLASS_MAIN_NAME L"MyMainWindow"
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3

PMainWindow::PMainWindow(wchar_t* title, uint32_t width, uint32_t height, HINSTANCE hinst, HWND parent)
:
PWindow::PWindow(title, width, height, hinst, parent)
{
}

LRESULT PMainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugString(L"CMainWindow::WndProc -> WM_CREATE\n");
            install_menubar();
            break;
        case WM_COMMAND:
			OutputDebugString(L"CMainWindow::WndProc -> WM_COMMAND\n");
            switch(LOWORD(wparam)) {
				case IDM_FILE_NEW:
					OutputDebugString(L"IDM_FILE_NEW\n");
					break;
				case IDM_FILE_OPEN:
					OutputDebugString(L"IDM_FILE_OPEN\n");
					break;
                case IDM_FILE_QUIT:
					OutputDebugString(L"IDM_FILE_QUIT\n");
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

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void PMainWindow::install_menubar()
{
    HMENU hMenubar = ::CreateMenu();
    HMENU hMenu = ::CreateMenu();

    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
    ::AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    ::AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

    ::AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
    ::SetMenu(mHwnd, hMenubar);
    ::DrawMenuBar(mHwnd);
}