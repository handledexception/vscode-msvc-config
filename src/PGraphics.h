#pragma once

#define GFX_DLL_EXPORT __declspec( dllexport )

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

typedef struct GraphicsDevice GraphicsDevice;
typedef struct GraphicsDesc GraphicsDesc;
typedef struct ZBuffer ZBuffer;
typedef struct Texture2d Texture2d;
typedef struct Swapchain Swapchain;
typedef struct GraphicsCore GraphicsCore;
typedef struct MatrixBuffer MatrixBuffer;
typedef struct Vertex Vertex;
typedef struct PixelShader PixelShader;
typedef struct VertexShader VertexShader;
typedef struct GraphicsAdapter GraphicsAdapter;
typedef struct Monitor Monitor;

struct GraphicsDesc {
	uint32_t m_canvas_width;
	uint32_t m_canvas_height;
	uint32_t m_render_width;
	uint32_t m_render_height;
	uint32_t m_fps_num;
	uint32_t m_fps_den;
	uint32_t m_adapter_index;
	bool m_enable_zbuffer;
	bool m_orthographic;
	DXGI_FORMAT m_dxgi_format;
};

struct Texture2d {
	std::shared_ptr<GraphicsDevice> m_gfx_device;
	ComPtr<ID3D11Texture2D> m_d3d11_texture;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	uint32_t m_width;
	uint32_t m_height;
	DXGI_FORMAT m_dxgi_format;
};

struct ZBuffer {
	std::shared_ptr<GraphicsDevice> m_gfx_device;
	ComPtr<ID3D11Texture2D> m_d3d11_texture;
	ComPtr<ID3D11DepthStencilView> m_zs_view;
	uint32_t m_width;
	uint32_t m_height;
	DXGI_FORMAT m_dxgi_format;
	D3D11_TEXTURE2D_DESC m_tex_desc = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC m_zs_view_desc = {};

	void Init(bool enable);
};

struct GraphicsDevice {
	void InitFactory(uint32_t adapter_idx);
	void InitDevice(uint32_t adapter_idx);
	void SetRasterState();
	void SetRenderTarget(Texture2d *texture, ZBuffer* zstencil);
	void ResetViewport(uint32_t width, uint32_t height);
	bool InitShaders(const wchar_t *vs_path, const wchar_t *ps_path);
	void SetProjection(uint32_t width, uint32_t height, bool ortho);
	GraphicsDevice(uint32_t adapter_idx);
	~GraphicsDevice();

	ComPtr<IDXGIFactory1> m_dxgi_factory;
	ComPtr<IDXGIAdapter1> m_dxgi_adapter;
	ComPtr<ID3D11Device> m_d3d11_device;
	ComPtr<ID3D11DeviceContext> m_d3d11_context;
	ComPtr<ID3D11RasterizerState> m_d3d11_raster;

	D3D11_VIEWPORT m_viewport;
	std::shared_ptr<Swapchain> m_swapchain;
	Texture2d* m_curr_render_target;
	ZBuffer *m_curr_zstencil_buf;

	VertexShader *m_vertex_shader;
	PixelShader *m_pixel_shader;

	struct Matrix m_model_matrix;
	struct Matrix m_view_matrix;
	struct Matrix m_proj_matrix;
};

struct Swapchain {
	void Init(uint32_t cx, uint32_t cy, bool z_enable);
	void InitRenderTarget(uint32_t cx, uint32_t cy);
	void InitZStencilBuffer(bool enable, uint32_t cx, uint32_t cy);
	void Resize(uint32_t cx, uint32_t cy, bool z_enable);

	Swapchain(std::shared_ptr<GraphicsDevice>& device, GraphicsDesc& gfx_desc, HWND hwnd);
	~Swapchain();

	HWND m_hwnd;
	DXGI_SWAP_CHAIN_DESC m_swap_desc = {};
	ComPtr<IDXGISwapChain> m_dxgi_swapchain;
	std::shared_ptr<GraphicsDevice> m_gfx_device;
	Texture2d m_render_target;
	ZBuffer m_zstencil;
	GraphicsDesc m_gfx_desc;
};

struct GraphicsCore {
	std::shared_ptr<GraphicsDevice> m_gfx_device;
	GraphicsDesc m_gfx_desc;
	bool m_done;
};

template <class T> struct Rectangle {
	T x;
	T y;
	T cx;
	T cy;
};

struct Vertex {
	struct Vector4 m_position;
	struct Vector2 m_tex_coord;
};

struct MatrixBuffer {
	struct Matrix m_model_matrix;
	struct Matrix m_view_matrix;
	struct Matrix m_proj_matrix;
};

struct VertexShader {
	VertexShader(std::shared_ptr<GraphicsDevice>& device) { m_gfx_device = device; };
	void CreateVertexBuffer(void* data, size_t elem_size, bool dynamic_usage=false);
	HRESULT CompileVertexShader(const wchar_t* shader_path);
	void InitInputElements();
	void InitMatrixBuffer();
	void SetParameters(struct Matrix& model, struct Matrix& view, struct Matrix& proj);

	ComPtr<ID3D11Buffer> m_vertex_buffer;
	ComPtr<ID3D11Buffer> m_matrix_buffer;
	ComPtr<ID3D11InputLayout> m_vertex_layout;
	ComPtr<ID3D11VertexShader> m_shader;
	ComPtr<ID3DBlob> m_shader_blob;
	ComPtr<ID3DBlob> m_error_blob;
	std::shared_ptr<GraphicsDevice> m_gfx_device;
};

struct PixelShader {
	PixelShader(std::shared_ptr<GraphicsDevice>& device) { m_gfx_device = device; };
	HRESULT CompilePixelShader(const wchar_t *shader_path);
	void InitSamplerState();

	ComPtr<ID3D11PixelShader> m_shader;
	ComPtr<ID3D11SamplerState> m_sampler_state;
	ComPtr<ID3DBlob> m_shader_blob;
	ComPtr<ID3DBlob> m_error_blob;
	std::shared_ptr<GraphicsDevice> m_gfx_device;
};

struct Monitor {
	int32_t m_index;
	std::wstring m_name;
	Rectangle<int32_t> m_geometry;
	DXGI_MODE_ROTATION m_rotation;
};

struct GraphicsAdapter {
	int32_t m_index;
	std::wstring m_name;
	std::vector<Monitor> m_monitors;
};

static std::unique_ptr<GraphicsCore> graphics_core;
static std::vector<GraphicsAdapter> graphics_adapters;
static std::thread render_thread;

void enumerate_monitors(GraphicsAdapter& adapter, ComPtr<IDXGIAdapter1> dxgi_adapter);
void enumerate_adapters();
void render_thread_main();
GFX_DLL_EXPORT void reset_graphics(GraphicsDesc& gfx_desc, HWND view_wnd);
GFX_DLL_EXPORT void shutdown_graphics();

}; // namespace gfx
