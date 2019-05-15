#include "PWindow.h"

class PMainWindow : public PWindow {

public:
    PMainWindow(wchar_t* title, uint32_t width, uint32_t height, HINSTANCE hinst, HWND parent);
	~PMainWindow() {};
    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    void install_menubar();    
};