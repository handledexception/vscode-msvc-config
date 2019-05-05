// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "cwindow.h"
#include "cmainwindow.h"
#include "cwindowmanager.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLine,
    int cmdShow)
{
    CWindowManager *mgr = new CWindowManager(hInstance, L"My Application");
     mgr->DoRegisterClass();

    CMainWindow *mw = new CMainWindow(800, 600, L"Default Window", nullptr);
    //CWindow *w2 = new CWindow(320, 240, nullptr, mw->Handle());
    mw->Show(true);
    //w2->Show(true);

    mgr->AddChildWindow(mw);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    mgr->FreeChildWindows();
    delete mgr;
    mgr = nullptr;

    return 0;
}