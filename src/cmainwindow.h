#include "cwindow.h"

class CMainWindow : public CWindow {

public:
    CMainWindow(int32_t width, int32_t height, const wchar_t *title, HWND parent) : CWindow(width, height, title, parent) { mHwnd = createHwnd(this, width, height, title, parent); }
    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    HWND createHwnd(CWindow *self, int32_t width, int32_t height, const wchar_t *title, HWND parent);
    void initializeMenus();
};