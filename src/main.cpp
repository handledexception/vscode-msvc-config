// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "cwindow.h"
#include "cwindowmanager.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLine,
    int cmdShow)
{    
    CWindowManager *mgr = new CWindowManager(hInstance, L"My Application");
     mgr->DoRegisterClass();

    CWindow *w = new CWindow(800, 600);
    w->Show(true);

    mgr->AddChildWindow(w);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}