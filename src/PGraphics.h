#pragma once

#include "PWindow.h"
#include "PObject.h"

#include <cstdint>
#include <thread>
#include <vector>

#include <Windows.h>
#include <wrl/client.h> // ComPtr
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

namespace gfx {

typedef struct gfx_rect gfx_rect_t;
typedef struct gfx_video_info gfx_video_info_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_zstencil_buffer gfx_zstencil_buffer_t;
typedef struct gfx_texture_2d gfx_texture_2d_t;
typedef struct gfx_vertex gfx_vertex_t;
typedef struct gfx_system gfx_system_t;

struct gfx_vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex_coord;
};

struct gfx_system {
	gfx_device_t* device;
	gfx_video_info_t* vid_info;
	bool gfx_done;
};

struct gfx_rect {
	int32_t x;
	int32_t y;
	int32_t cx;
	int32_t cy;
};

struct gfx_video_info {
	uint32_t canvas_width;
	uint32_t canvas_height;
	uint32_t render_width;
	uint32_t render_height;
	uint32_t fps_num;
	uint32_t fps_den;
	uint32_t adapter_index;
};

struct gfx_zstencil_buffer {
	gfx_device_t* device;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11DepthStencilView> zs_view;
	uint32_t width;
	uint32_t height;
	DXGI_FORMAT dxgi_format;
	D3D11_TEXTURE2D_DESC tex_desc = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC zs_view_desc = {};

	void Init();
};

struct gfx_texture_2d {
	gfx_device_t* device;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> rtv;
	uint32_t width;
	uint32_t height;
	DXGI_FORMAT dxgi_format;
};

struct gfx_swapchain : PObject {
	HWND hwnd;
	DXGI_SWAP_CHAIN_DESC swap_desc = {};
	ComPtr<IDXGISwapChain> chain;
	gfx_device_t* device;
	gfx_texture_2d target_tex;
	gfx_zstencil_buffer zstencil;

	void Init(uint32_t cx, uint32_t cy);
	void InitRenderTarget(uint32_t cx, uint32_t cy);
	void InitZStencilBuffer(uint32_t cx, uint32_t cy);
	void Resize(uint32_t cx, uint32_t cy);
	gfx_swapchain(gfx_device_t *device, uint32_t width, uint32_t height, uint32_t fps_num, uint32_t fps_den, HWND hwnd);
	~gfx_swapchain();
};

struct gfx_device : PObject {
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	gfx_rect viewport;
	gfx_swapchain* swapchain;
	gfx_texture_2d* current_render_target;
	
	void InitFactory(uint32_t adapter_idx);
	void InitDevice(uint32_t adapter_idx);

	gfx_device(uint32_t adapter_idx);
	~gfx_device();
};

struct gfx_monitor {
	int32_t index;
	const wchar_t* name;
	gfx_rect geometry;
};

struct gfx_adapter {
	int32_t index;
	const wchar_t* name;
	std::vector<gfx_monitor> monitors;
};

static gfx_system* graphics_system;

static std::vector<gfx_adapter> adapters;
static std::thread video_thread;

void enumerate_monitors();
void enumerate_adapters();
void video_thread_loop();
void reset_video(gfx_video_info_t* vid_info, HWND view_wnd);
void shutdown_video();

};