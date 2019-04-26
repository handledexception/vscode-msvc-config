#include <windows.h>
#include <cstdint>

class CWindow {

public:
    CWindow(int32_t width, int32_t height);
    ~CWindow();
    HWND Handle() { return m_Hwnd; }
private:
    static void registerClass();
    static LRESULT CALLBACK staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static HWND createHwnd(CWindow *self, int32_t width, int32_t height);
    void initializeMenus();

private:
    unsigned int m_Width, m_Height;
    HWND m_Hwnd;
};