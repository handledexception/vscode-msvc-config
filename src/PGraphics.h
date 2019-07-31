#pragma once

#include "PWindow.h"

#include <cstdint>
#include <memory>
#include <thread>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Windows.h>
#include <wrl/client.h> // ComPtr
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "SimpleMath.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Rectangle;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using Microsoft::WRL::ComPtr;

namespace gfx
{
typedef struct gfx_video_info gfx_video_info_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_zstencil_buffer gfx_zstencil_buffer_t;
typedef struct gfx_texture_2d gfx_texture_2d_t;
typedef struct gfx_vertex gfx_vertex_t;
typedef struct gfx_matrix_buffer gfx_matrix_buffer_t;

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

typedef struct gfx_vertex_shader gfx_vertex_shader_t;
typedef struct gfx_pixel_shader gfx_pixel_shader_t;
typedef struct gfx_core gfx_core_t;

struct gfx_vertex {
	struct Vector4 position;
	struct Vector2 tex_coord;
};

struct gfx_matrix_buffer {
	struct Matrix m_view_matrix;
	struct Matrix m_proj_matrix;
};

struct gfx_system {
	std::shared_ptr<gfx_device_t> m_gfx_device;
	gfx_video_info_t m_video_info;
	bool gfx_done;
};

struct gfx_vertex_shader {
	ComPtr<ID3D11Buffer> m_vertex_buffer;
	ComPtr<ID3D11Buffer> m_matrix_buffer;
	ComPtr<ID3D11InputLayout> m_vertex_layout;
	ComPtr<ID3D11VertexShader> m_shader;
	ComPtr<ID3DBlob> m_shader_blob;
	ComPtr<ID3DBlob> m_error_blob;
	std::shared_ptr<gfx_device_t> m_device;

	gfx_vertex_shader(std::shared_ptr<gfx_device_t> device) { m_device = device; };
	void CreateVertexBuffer(void *data, size_t elem_size, bool dynamic_usage=false);
	HRESULT CompileVertexShader(const wchar_t *shader_path);
	void InitInputElements();
	void InitMatrixBuffer();
	void SetParameters(struct Matrix &world, struct Matrix &proj);
};

struct gfx_pixel_shader {
	ComPtr<ID3D11PixelShader> m_shader;
	ComPtr<ID3D11SamplerState> m_sampler_state;
	ComPtr<ID3DBlob> m_shader_blob;
	ComPtr<ID3DBlob> m_error_blob;
	std::shared_ptr<gfx_device_t> m_device;

	gfx_pixel_shader(std::shared_ptr<gfx_device_t> device) { m_device = device; };
	HRESULT CompilePixelShader(const wchar_t *shader_path);
	void InitSamplerState();
};

template <class T> struct gfx_rect {
	T x;
	T y;
	T cx;
	T cy;
};

//struct gfx_zstencil_buffer {
//	std::shared_ptr<gfx_device_t> m_gfx_device;
//	ComPtr<ID3D11Texture2D> texture;
//	ComPtr<ID3D11DepthStencilView> zs_view;
//	uint32_t width;
//	uint32_t height;
//	DXGI_FORMAT dxgi_format;
//	D3D11_TEXTURE2D_DESC tex_desc = {};
//	D3D11_DEPTH_STENCIL_VIEW_DESC zs_view_desc = {};
//
//	void Init();
//};

struct gfx_texture_2d {
	std::shared_ptr<gfx_device_t> m_gfx_device;
	ComPtr<ID3D11Texture2D> m_d3d11_texture;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	uint32_t m_width;
	uint32_t m_height;
	DXGI_FORMAT m_dxgi_format;
};

struct gfx_swapchain {
	HWND m_hwnd;
	DXGI_SWAP_CHAIN_DESC m_swap_desc = {};
	ComPtr<IDXGISwapChain> m_dxgi_swapchain;
	std::shared_ptr<gfx_device_t> m_gfx_device;
	gfx_texture_2d m_render_target_tex;
	//gfx_zstencil_buffer m_zstencil;
	gfx_video_info_t m_vid_info;

	void Init(uint32_t cx, uint32_t cy);
	void InitRenderTarget(uint32_t cx, uint32_t cy);
	//void InitZStencilBuffer(uint32_t cx, uint32_t cy);
	void Resize(uint32_t cx, uint32_t cy);

	gfx_swapchain(std::shared_ptr<gfx_device_t> device, gfx_video_info_t& vid_info, HWND hwnd);
	~gfx_swapchain();
};

struct gfx_device {
	ComPtr<IDXGIFactory1> m_dxgi_factory;
	ComPtr<IDXGIAdapter1> m_dxgi_adapter;
	ComPtr<ID3D11Device> m_d3d11_device;
	ComPtr<ID3D11DeviceContext> m_d3d11_context;
	ComPtr<ID3D11RasterizerState> m_d3d11_raster;

	D3D11_VIEWPORT m_viewport;
	std::shared_ptr<gfx_swapchain_t> m_swapchain;
	gfx_texture_2d *m_curr_render_target;
	//gfx_zstencil_buffer *m_curr_zstencil_buf;

	gfx_vertex_shader *m_vertex_shader;
	gfx_pixel_shader *m_pixel_shader;

	struct Matrix m_view_matrix;
	struct Matrix m_ortho_matrix;

	void InitFactory(uint32_t adapter_idx);
	void InitDevice(uint32_t adapter_idx);
	void SetRasterState();
	void SetRenderTarget(gfx_texture_2d_t *texture);
	void ResetViewport(uint32_t width, uint32_t height);
	bool InitShaders(const wchar_t *vs_path, const wchar_t *ps_path);
	gfx_device(uint32_t adapter_idx);
	~gfx_device();
};

struct gfx_monitor {
	int32_t m_index;
	std::wstring m_name;
	gfx_rect<int32_t> m_geometry;
	DXGI_MODE_ROTATION m_rotation;
};

struct gfx_adapter {
	int32_t m_index;
	std::wstring m_name;
	std::vector<gfx_monitor> m_monitors;
};

static std::unique_ptr<gfx_system> graphics_system;

static std::vector<gfx_adapter> gfx_adapters;
static std::thread video_thread;

void enumerate_monitors(gfx_adapter& gfx_adapter, ComPtr<IDXGIAdapter1> dxgi_adapter);
void enumerate_adapters();
void video_thread_loop();
void reset_graphics(gfx_video_info_t& vid_info, HWND view_wnd);
void shutdown_graphics();

}; // namespace gfx
