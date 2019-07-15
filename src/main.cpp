// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <cstdint>

#include "PMainWindow.h"
#include "PWindow.h"
#include "PGraphics.h"

// #include "Graphics.h"
// #include "GraphicsD3D11.h"

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#define RENDER_WIDTH 640
#define RENDER_HEIGHT 360

void init_preview_video_info(gfx::gfx_video_info_t& vid_info)
{
	vid_info.m_adapter_index = 0;
	vid_info.m_canvas_height = CANVAS_HEIGHT;
	vid_info.m_canvas_width = CANVAS_WIDTH;
	vid_info.m_fps_den = 1;
	vid_info.m_fps_num = 30;
	vid_info.m_render_height = RENDER_HEIGHT;
	vid_info.m_render_width = RENDER_WIDTH;
	vid_info.m_dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	wprintf(L"WinMain\n");

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	PMainWindow* main_wnd = new PMainWindow(L"PaulApp", CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800, hInstance, nullptr);

	main_wnd->Init(
		WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW, /* ex style */
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	  /* style */
	);
	main_wnd->Show(true);

	PWindow* view_wnd = new PWindow(L"PaulDX11", 20, 20, RENDER_WIDTH, RENDER_HEIGHT, hInstance, main_wnd->GetHandle());
	view_wnd->Init(NULL, WS_CHILD | WS_CLIPCHILDREN);
	view_wnd->Show(true);

	// pas::IGraphicsDevice* graphics_device = new pas::D3D11GraphicsDevice();
	// graphics_device->Create(0);
	
	// preview view
	struct gfx::gfx_video_info pvi;
	init_preview_video_info(pvi);
	gfx::reset_graphics(pvi, view_wnd->GetHandle());

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete view_wnd;
	view_wnd = nullptr;
	delete main_wnd;
	main_wnd = nullptr;

	gfx::shutdown_graphics();

	CoUninitialize();

	return 0;
}