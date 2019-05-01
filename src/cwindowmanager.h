#pragma once

#include <windows.h>
#include <vector>
#include "cwindow.h"

static HWND gDummyWindow;

class CWindowManager {

public:
    CWindowManager(HINSTANCE hInstance, wchar_t *appName);
    ~CWindowManager() { };
    void DoRegisterClass();
    void AddChildWindow(CWindow *window);
    LRESULT ChildWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
private:
    void createDummyWindow();
    static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE mInstance;
    wchar_t *mAppName;
    std::vector<CWindow *> mChildWindows;
};