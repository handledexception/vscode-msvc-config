// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <cstdint>

#include "PGraphics.h"
#include "PMainWindow.h"
#include "PWindow.h"

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#define RENDER_WIDTH 640
#define RENDER_HEIGHT 360

void init_graphics_desc(gfx::GraphicsDesc& desc)
{
	desc.m_adapter_index = 0;
	desc.m_canvas_height = CANVAS_HEIGHT;
	desc.m_canvas_width = CANVAS_WIDTH;
	desc.m_fps_den = 1;
	desc.m_fps_num = 30;
	desc.m_render_height = RENDER_HEIGHT;
	desc.m_render_width = RENDER_WIDTH;
	desc.m_enable_zbuffer = true;
	desc.m_orthographic = true;
	desc.m_dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	wprintf(L"WinMain\n");

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	PMainWindow* main_wnd = new PMainWindow(
		L"PaulApp", CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800, hInstance, nullptr);

	main_wnd->Init(
		WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW, /* ex style */
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	  /* style */
	);
	main_wnd->Show(true);

	PWindow* view_wnd = new PWindow(
		L"PaulDX11", 20, 20, RENDER_WIDTH, RENDER_HEIGHT, hInstance, main_wnd->GetHandle());
	view_wnd->Init(NULL, WS_CHILD | WS_CLIPCHILDREN);
	view_wnd->Show(true);

	// preview view
	struct gfx::GraphicsDesc gfx_desc;
	init_graphics_desc(gfx_desc);
	gfx::reset_graphics(gfx_desc, view_wnd->GetHandle());

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	gfx::shutdown_graphics();

	delete view_wnd;
	view_wnd = nullptr;
	delete main_wnd;
	main_wnd = nullptr;

	CoUninitialize();

	return 0;
}