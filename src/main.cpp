// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "cwindow.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLine,
    int cmdShow)
{
    CWindow *w = new CWindow(640, 480);
    HWND handle = w->Handle();
    // CWindow *w2 = new CWindow(800, 600);
    // HWND handle2 = w2->Handle();

    ShowWindow(handle, SW_SHOWNORMAL);
    UpdateWindow(handle);
    // ShowWindow(handle2, SW_SHOWNORMAL);
    // UpdateWindow(handle2);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}