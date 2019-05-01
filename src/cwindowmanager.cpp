#include "cwindowmanager.h"

CWindowManager::CWindowManager(HINSTANCE hInstance, wchar_t *appName)
:
mInstance(hInstance),
mAppName(appName),
mChildWindows(0)
{
}

void CWindowManager::DoRegisterClass()
{
    //HMODULE instance = GetModuleHandle(nullptr);
    WNDCLASSEX wcex;

    // window already registered
    if (GetClassInfoEx(mInstance, mAppName, &wcex)) {
        return;
    }

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_DBLCLKS;
    wcex.lpfnWndProc   = &CWindowManager::staticWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = mInstance;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = mAppName;
    wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0) {
        OutputDebugString(L"Error registering window class!");
        return;
    }

    createDummyWindow();
}

void CWindowManager::AddChildWindow(CWindow *window)
{
    mChildWindows.push_back(window);
}

LRESULT CWindowManager::ChildWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    for (auto w : mChildWindows) {
        if (w->Handle() == hwnd) {
            return w->WndProc(hwnd, msg, wparam, lparam);
        }
    }

    return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

void CWindowManager::createDummyWindow()
{
    const wchar_t *dummyName = L"MyAppDummy";
    gDummyWindow = ::CreateWindow(mAppName, L"", WS_OVERLAPPEDWINDOW, -100, -100, 10, 10, 0, 0, mInstance, 0);
    ::SetWindowLongPtr(gDummyWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

LRESULT CALLBACK CWindowManager::staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    CWindowManager *mgr = reinterpret_cast<CWindowManager *>(::GetWindowLongPtr(gDummyWindow, GWLP_USERDATA));
    if (mgr) {
        return mgr->ChildWindowProc(hwnd, msg, wparam, lparam);
    }

    return ::DefWindowProc(hwnd, msg, wparam, lparam);

    /*
    CWindow *wnd = nullptr;
    if (msg == WM_CREATE) {
        wnd = reinterpret_cast<CWindow *>(
            reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams
        );

        if (wnd && wnd->Handle() == nullptr) {
            wnd->SetHandle(hwnd);
        }
    }
    else {
        wnd = reinterpret_cast<CWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (wnd && wnd->Handle() != hwnd) {
            wnd = nullptr;
        }
    }

    if (wnd) {
        wnd->SetHandle(hwnd);
        return wnd->wndProc(hwnd, msg, wparam, lparam);
    }
    else {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    */
}