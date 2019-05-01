#pragma once

#include <windows.h>
#include <cstdint>

class CWindow {

public:
    CWindow(int32_t width, int32_t height);
    ~CWindow();
    HWND Handle() { return mHwnd; }
    void SetHandle(HWND hwnd) { mHwnd = hwnd; }
    void Show(bool show);
    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
    //static void registerClass();
    //static LRESULT CALLBACK staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    HWND createHwnd(CWindow *self, int32_t width, int32_t height);
    void initializeMenus();

private:
    unsigned int mWidth, mHeight;
    HWND mHwnd;
};