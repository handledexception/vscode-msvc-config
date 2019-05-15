// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <cstdint>

#include "PMainWindow.h"
#include "PWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
    PMainWindow* pwnd = new PMainWindow(L"PaulApp", 1280, 800, hInstance, nullptr);
    pwnd->Init(
        WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW, /* ex style */
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	  /* style */
    );
	pwnd->Show(true);
    
	PWindow* pwny = new PWindow(L"PaulSubWindow", 640, 360, hInstance, pwnd->GetHandle());
    pwny->Init(WS_EX_OVERLAPPEDWINDOW, WS_CHILD | WS_CLIPCHILDREN);
	pwny->Show(true);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}