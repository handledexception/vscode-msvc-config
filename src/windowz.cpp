#include <iostream>

#include "windowz.h"

Window::Window(HWND hwnd)
{
    MessageBox(hwnd, L"Hello, world!", NULL, NULL);
}

Window::~Window()
{
}