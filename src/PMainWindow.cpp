#include "PMainWindow.h"

#define WND_CLASS_MAIN_NAME L"MyMainWindow"
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_HELP_ABOUT 4

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
				case IDM_HELP_ABOUT:
					auto res = MessageBox(mHwnd, L"About my app", L"About", 1 | MB_ICONINFORMATION);
					OutputDebugString(L"CMainWindow::WndProc -> IDM_HELP_ABOUT");
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
    HMENU hFileMenu = ::CreateMenu();
	HMENU hHelpMenu = ::CreateMenu();
    
	::AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, L"&New");
    ::AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
    ::AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    ::AppendMenu(hFileMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

	::AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, L"&About");

    ::AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
	::AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelpMenu, L"&Help");
    
	::SetMenu(mHwnd, hMenubar);
    
	::DrawMenuBar(mHwnd);
}