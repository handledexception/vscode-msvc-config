#include <windows.h>
#include <stdio.h>
#include <stdint.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

void FillWindowClass(WNDCLASSEX *wc, HINSTANCE instance, const char *wc_name, void *wndproc)
{
    wc->cbSize = sizeof(WNDCLASSEX);
    wc->style = 0;
    wc->lpfnWndProc = wndproc;
    wc->cbClsExtra = 0;
    wc->cbWndExtra = 0;
    wc->hInstance = instance;
    wc->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc->hCursor = LoadCursor(NULL, IDC_ARROW);
    wc->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc->lpszMenuName = NULL;
    wc->lpszClassName = wc_name;
    wc->hIconSm = LoadIcon(NULL, IDI_APPLICATION);
}

int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE prev_instance,
    LPSTR cmd_line,
    int cmd_show)
{
    HWND hwnd;
    MSG msg;
    const char *appname = "My Application";
    const char *window_title = "My Application";

    WNDCLASSEX wc = { 0 };
    FillWindowClass(&wc, instance, appname, WndProc);
    if (!RegisterClassEx(&wc)) {
        OutputDebugStringA("Error registering window class");
        return -1;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        appname,
        "Window title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        240,
        120,
        NULL,
        NULL,
        instance,
        NULL);

    if (hwnd == NULL) {
        OutputDebugStringA("Error creating window");
        return -1;
    }

    ShowWindow(hwnd, cmd_show);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}