#include <iostream>

#include "cwindow.h"

#define WND_CLASS_NAME L"My Application"
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3

CWindow::CWindow(int32_t width, int32_t height)
{
    registerClass();

    m_Hwnd = createHwnd(this, width, height);

    SetWindowLongPtr(m_Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

CWindow::~CWindow()
{
    if (m_Hwnd) {
        DestroyWindow(m_Hwnd);
    }
}

void CWindow::registerClass()
{
    HMODULE instance = GetModuleHandle(nullptr);

    WNDCLASSEX wcex;

    // window already registered
    if (GetClassInfoEx(instance, WND_CLASS_NAME, &wcex)) {
        return;
    }

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_DBLCLKS;
    wcex.lpfnWndProc   = &CWindow::staticWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = instance;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = WND_CLASS_NAME;
    wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0) {
        OutputDebugString(L"Error registering window class!");
        return;
    }
}

HWND CWindow::createHwnd(CWindow *self, int32_t width, int32_t height)
{
    OutputDebugString(L"Creating window...\n");
    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        WND_CLASS_NAME,
        L"",
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
    HMENU hMenubar = CreateMenu();
    HMENU hMenu = CreateMenu();

    AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
    AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");
    SetMenu(m_Hwnd, hMenubar);
}

LRESULT CWindow::wndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugString(L"Creating window...");
            this->initializeMenus();
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam)) {
                case IDM_FILE_QUIT:
                    PostMessage(m_Hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(m_Hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(m_Hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK CWindow::staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    CWindow *wnd = nullptr;
    if (msg == WM_CREATE) {
        wnd = reinterpret_cast<CWindow *>(
            reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams
        );

        if (wnd && wnd->m_Hwnd == nullptr) {
            wnd->m_Hwnd = hwnd;
        }
    }
    else {
        wnd = reinterpret_cast<CWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (wnd && wnd->m_Hwnd != hwnd) {
            wnd = nullptr;
        }
    }

    if (wnd) {
        return wnd->wndProc(msg, wparam, lparam);
    }
    else {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}
