#include <windows.h>

class Window {

public:
    Window(HWND hwnd);
    ~Window();

private:
    unsigned int width, height;
};