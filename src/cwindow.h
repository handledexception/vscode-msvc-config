#pragma once

#include <windows.h>
#include <cstdint>

#define WND_CLASS_NAME L"My Application"

class CWindow {

public:    
    CWindow(int32_t width, int32_t height, const wchar_t *title, HWND parent);
    ~CWindow();
    HWND Handle() { return mHwnd; }
    void SetHandle(HWND hwnd) { mHwnd = hwnd; }
    void Show(bool show);
    virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

private:
    virtual HWND createHwnd(CWindow *self, int32_t width, int32_t height, const wchar_t *title, HWND parent) = 0;
  //  void initializeMenus();

protected:
    unsigned int mWidth, mHeight;
    HWND mHwnd;
};