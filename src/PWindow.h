#pragma once

#include <Windows.h>
#include <cstdint>

class PWindow {

public:
	PWindow(wchar_t* title, uint32_t cx, uint32_t cy, uint32_t width, uint32_t height, HINSTANCE hinst, HWND parent);
	bool Init(int32_t hwnd_style_ex, int32_t hwnd_style);
	virtual ~PWindow();
	virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void Show(bool show);
	HWND GetHandle() { return m_hwnd; }
	void SetHandle(HWND hwnd) { m_hwnd = hwnd; }

private:
	void register_class();
	HWND create_hwnd(int32_t exStyle, int32_t style);
	static LRESULT CALLBACK static_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

protected:
	HINSTANCE m_instance;
	HWND m_hwnd, m_parent_hwnd;
	uint32_t m_cx, m_cy;
	uint32_t m_width, m_height;
	wchar_t* m_window_title;
	wchar_t* m_windowclass_name;
};
