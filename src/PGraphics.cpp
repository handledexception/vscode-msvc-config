#include "PGraphics.h"
#include "PLogger.h"

#include <exception>
#include <map>
#include <string>

// #pragma comment(lib, "dxgi.lib")
// #pragma comment(lib, "d3d11.lib")

namespace gfx {

void video_thread_loop()
{
	OutputDebugString(L"video_thread_loop BEGIN\n");
	if (!graphics_system)
		return;

	while (!graphics_system->gfx_done) {
		// render...
		ID3D11RenderTargetView* rtv = graphics_system->device->swapchain->target_tex.rtv.Get();
		const float clear_color[4] = {
			0.0, 1.0, 0.0, 1.0,
		};
		if (rtv) {
			graphics_system->device->context->ClearRenderTargetView(rtv, clear_color);
			graphics_system->device->swapchain->chain->Present(1, 0);
		}

		// OutputDebugString(L"video_thread_loop...\n");
	}

	OutputDebugString(L"video_thread_loop END\n");
}

void reset_video(gfx_video_info_t* vid_info, HWND view_wnd)
{
	HRESULT hr;
	ComPtr<ID3D11Buffer> back_buffer;

	if (!graphics_system)
		graphics_system = new gfx_system();

	if (vid_info) {
		struct gfx_device* gfx_dev = new gfx_device(vid_info->adapter_index);

		gfx_dev->swapchain = new gfx_swapchain(
			gfx_dev,
			vid_info->canvas_width,
			vid_info->canvas_height,
			vid_info->fps_num,
			vid_info->fps_den,
			view_wnd
		);

		graphics_system->device = gfx_dev;
		graphics_system->vid_info = vid_info;
	}

	graphics_system->gfx_done = false;
	video_thread = std::thread(video_thread_loop);
}

void shutdown_video()
{
	if (graphics_system)
		graphics_system->gfx_done = true;
	video_thread.join();
}

void gfx_zstencil_buffer::Init()
{
	HRESULT hr;
	::ZeroMemory(&tex_desc, sizeof(tex_desc));
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = dxgi_format;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;

	hr = device->device->CreateTexture2D(&tex_desc, NULL, texture.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("Failed to create zstencil texture!\n");

	::ZeroMemory(&zs_view_desc, sizeof(zs_view_desc));
	zs_view_desc.Format = dxgi_format;
	zs_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = device->device->CreateDepthStencilView(texture.Get(), &zs_view_desc, zs_view.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("Failed to create zstencil view!\n");
}

void gfx_swapchain::InitRenderTarget(uint32_t cx, uint32_t cy)
{
	HRESULT hr;
	target_tex.device = device;
	target_tex.width = cx;
	target_tex.height = cy;
	target_tex.dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM; //TODO: What situations call for using a different render target format?

	hr = chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)target_tex.texture.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("IDXGISwapChain::GetBuffer ID3D11Texture2D failed for gfx_swapchain!\n");

	hr = device->device->CreateRenderTargetView(target_tex.texture.Get(), NULL, target_tex.rtv.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("ID3D11Device::CreateRenderTargetView failed for gfx_swapchain!\n");
}

void gfx_swapchain::InitZStencilBuffer(uint32_t cx, uint32_t cy)
{
	zstencil.device = device;
	zstencil.width = cx;
	zstencil.height = cy;
	zstencil.dxgi_format = DXGI_FORMAT_D24_UNORM_S8_UINT; //TODO: What situations call for using a zstencil different format?

	if (cx != 0 && cy != 0)
		zstencil.Init();
}

void gfx_swapchain::Init(uint32_t cx, uint32_t cy)
{
	InitRenderTarget(cx, cy);
	InitZStencilBuffer(cx, cy);
}

void gfx_swapchain::Resize(uint32_t cx, uint32_t cy)
{
	RECT client_rect;
	HRESULT hr;

	target_tex.texture.Reset();
	target_tex.rtv.Reset();
	zstencil.texture.Reset();
	zstencil.zs_view.Reset();

	if (cx == 0 || cy == 0) {
		::GetClientRect(hwnd, &client_rect);
		if (cx == 0) cx = client_rect.right;
		if (cy == 0) cy = client_rect.bottom;
	}

	hr = chain->ResizeBuffers(1, cx, cy, target_tex.dxgi_format, 0);
	if (FAILED(hr))
		throw std::exception("Failed to resize swap chain buffer!\n");

	InitRenderTarget(cx, cy);
	InitZStencilBuffer(cx, cy);
}

gfx_swapchain::gfx_swapchain(gfx_device_t *device, uint32_t width, uint32_t height, uint32_t fps_num, uint32_t fps_den, HWND hwnd)
:
device(device)
{
	HRESULT hr;

	// Create Swapchain Desc
	memset(&swap_desc, 0, sizeof(swap_desc));
	swap_desc.BufferCount = 1;
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_desc.BufferDesc.Width = width;
	swap_desc.BufferDesc.Height = height;
	swap_desc.BufferDesc.RefreshRate.Denominator = fps_den;
	swap_desc.BufferDesc.RefreshRate.Numerator = fps_num;
	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_desc.OutputWindow = hwnd;
	swap_desc.SampleDesc.Count = 1;
	swap_desc.Windowed = true;

	hr = device->factory->CreateSwapChain(device->device.Get(), &swap_desc, chain.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		throw std::exception("IDXGIFactory1::CreateSwapChain failed!\n");

	device->factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	//TODO: init texture for render target
	Init(width, height);
}

gfx_swapchain::~gfx_swapchain()
{
	chain.Detach();
}

void enumerate_monitors(IDXGIAdapter1* adapter)
{
	uint32_t idx = 0;
	ComPtr<IDXGIOutput> output;

	while(adapter->EnumOutputs(idx++, &output) == S_OK) {
		DXGI_OUTPUT_DESC desc;
		if (FAILED(output->GetDesc(&desc)))
			continue;

		PLogger::log(LOG_LEVEL::Debug, L"DX11 Monitor: %s\n", desc.DeviceName);
	}
}

//TODO: add adapter -> monitors to a std::map<adapter_name, monitors>
void enumerate_adapters()
{
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGIAdapter1> adapter;
	std::wstring name;
	uint32_t idx = 0;
	HRESULT hr;

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)factory.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

	while (factory->EnumAdapters1(idx++, adapter.GetAddressOf()) == S_OK) {
		DXGI_ADAPTER_DESC desc;

		hr = adapter->GetDesc(&desc);
		if (FAILED(hr))
			continue;

		PLogger::log(LOG_LEVEL::Debug, L"DXGI Adapter: %s\n", desc.Description);

		gfx_adapter adap;
		adap.index = idx;
		adap.name = desc.Description;
		adapters.push_back(adap);

		enumerate_monitors(adapter.Get());
	}
}

gfx_device::gfx_device(uint32_t adapter_idx)
:
current_render_target(nullptr),
swapchain(nullptr)
{
	InitFactory(adapter_idx);
	InitDevice(adapter_idx);
	context->OMSetRenderTargets(0, nullptr, nullptr);
}

gfx_device::~gfx_device()
{
	factory.Detach();
	context.Detach();
	device.Detach();
	adapter.Detach();
	delete swapchain;
}

void gfx_device::InitFactory(uint32_t adapter_idx)
{
	HRESULT hr = 0;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)factory.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

	hr = factory->EnumAdapters1(adapter_idx, &adapter);
	if (FAILED(hr))
		throw std::exception("IDXGIFactory2::EnumAdapters failed!");
}

const static D3D_FEATURE_LEVEL feature_levels[] =
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
};

void gfx_device::InitDevice(uint32_t adapter_idx)
{
	std::wstring adapter_name;
	DXGI_ADAPTER_DESC desc;
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_10_0;
	bool nv12_supported = false;

	memset(&viewport, 0, sizeof(viewport));

	uint32_t create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DX11_DEBUG
	create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = 0;

	hr = adapter->GetDesc(&desc);
	if (hr == S_OK)
		adapter_name = desc.Description;
	else
		adapter_name = L"Unknown DXGI Adapter";

	hr = D3D11CreateDevice(
		adapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		create_flags,
		feature_levels,
		sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(),
		&feature_level,
		context.ReleaseAndGetAddressOf()
	);
	if (FAILED(hr))
		throw std::exception("D3D11CreateDevice failed!");

	PLogger::log(LOG_LEVEL::Debug, L"DX11 InitDevice: %s, Feature Level: %u\n", adapter_name.c_str(), (uint32_t)feature_level);

	//TODO: check for NV12 support
}

};