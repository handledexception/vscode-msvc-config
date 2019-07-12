#pragma once

#include "PWindow.h"
#include "PObject.h"

#include <cstdint>
#include <thread>
#include <map>
#include <memory>
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

typedef struct gfx_rect_int32 gfx_rect_int32_t;
typedef struct gfx_rect_int64 gfx_rect_int64_t;
typedef struct gfx_rect_float gfx_rect_float_t;
typedef struct gfx_video_info gfx_video_info_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_zstencil_buffer gfx_zstencil_buffer_t;
typedef struct gfx_texture_2d gfx_texture_2d_t;
typedef struct gfx_vertex gfx_vertex_t;
typedef struct gfx_system gfx_system_t;
typedef struct gfx_object gfx_object_t;

struct gfx_video_info {
	uint32_t m_canvas_width;
	uint32_t m_canvas_height;
	uint32_t m_render_width;
	uint32_t m_render_height;
	uint32_t m_fps_num;
	uint32_t m_fps_den;
	uint32_t m_adapter_index;
	DXGI_FORMAT m_dxgi_format;
};

struct gfx_vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex_coord;
};

struct gfx_system {
	std::shared_ptr<gfx_device_t> m_gfx_device;
	gfx_video_info_t m_video_info;
	bool gfx_done;
};

template <class T> struct gfx_rect {
	T x;
	T y;
	T cx;
	T cy;
};

struct gfx_zstencil_buffer {
	std::shared_ptr<gfx_device_t> m_gfx_device;
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
	std::shared_ptr<gfx_device_t> m_gfx_device;
	ComPtr<ID3D11Texture2D> m_d3d11_texture;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	uint32_t m_width;
	uint32_t m_height;
	DXGI_FORMAT m_dxgi_format;
};

struct gfx_swapchain : PObject {
	HWND m_hwnd;
	DXGI_SWAP_CHAIN_DESC m_swap_desc = {};
	ComPtr<IDXGISwapChain> m_dxgi_swapchain;
	std::shared_ptr<gfx_device_t> m_gfx_device;
	gfx_texture_2d m_render_target_tex;
	gfx_zstencil_buffer m_zstencil;
	gfx_video_info_t m_vid_info;

	void Init(uint32_t cx, uint32_t cy);
	void InitRenderTarget(uint32_t cx, uint32_t cy);
	void InitZStencilBuffer(uint32_t cx, uint32_t cy);
	void Resize(uint32_t cx, uint32_t cy);
	gfx_swapchain(std::shared_ptr<gfx_device_t> device, gfx_video_info_t& vid_info, HWND hwnd);
	~gfx_swapchain();
};

struct gfx_device : PObject {
	ComPtr<IDXGIFactory1> m_dxgi_factory;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	gfx_rect<int32_t> viewport;
	gfx_swapchain* swapchain;
	gfx_texture_2d* current_render_target;
	
	void InitFactory(uint32_t adapter_idx);
	void InitDevice(uint32_t adapter_idx);

	gfx_device(uint32_t adapter_idx);
	~gfx_device();
};

struct gfx_monitor {
	int32_t index;
	std::wstring name;
	gfx_rect<int32_t> geometry;
	DXGI_MODE_ROTATION rotation;
};

struct gfx_adapter {
	int32_t index;
	std::wstring name;
	std::vector<gfx_monitor> monitors;
};

static std::unique_ptr<gfx_system> graphics_system;

static std::vector<gfx_adapter> gfx_adapters;
static std::thread video_thread;

void enumerate_monitors(gfx_adapter& gfx_adapter, ComPtr<IDXGIAdapter1> dxgi_adapter);
void enumerate_adapters();
void video_thread_loop();
void reset_graphics(gfx_video_info_t& vid_info, HWND view_wnd);
void shutdown_graphics();

};