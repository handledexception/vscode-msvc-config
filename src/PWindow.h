#pragma once

#include <Windows.h>
#include <cstdint>

class PWindow {

public:
	PWindow(wchar_t* title, uint32_t cx, uint32_t cy, uint32_t width, uint32_t height, HINSTANCE hinst, HWND parent);
	bool Init(int32_t hwndExStyle, int32_t hwndStyle);
	virtual ~PWindow();
	virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void Show(bool show);
	HWND GetHandle() { return mHwnd; }
	void SetHandle(HWND hwnd) { mHwnd = hwnd; }

private:
	void register_class();
	HWND create_hwnd(int32_t exStyle, int32_t style);
	static LRESULT CALLBACK static_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

protected:
	HINSTANCE mInstance;
	HWND mHwnd, mParentHwnd;
	uint32_t mPosX, mPosY;
	uint32_t mWidth, mHeight;
	wchar_t* mWindowTitle;
	wchar_t* mWindowClassName;
};
