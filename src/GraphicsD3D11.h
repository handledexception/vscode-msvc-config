#pragma once

#include "Graphics.h"

#include <Windows.h>
#include <wrl/client.h> // ComPtr
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

namespace pas {
	struct D3D11SwapChain;
	struct D3D11GraphicsDevice;
	struct D3D11Texture2D;
	struct D3D11ZStencil;

	struct D3D11ZStencil : public IZStencil {
		D3D11GraphicsDevice* m_graphics_device;
		DXGI_FORMAT m_dxgi_format;
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11DepthStencilView> m_zstencil_view;
		D3D11_TEXTURE2D_DESC m_texture_desc = {};
		D3D11_DEPTH_STENCIL_VIEW_DESC m_zstencil_view_desc = {};
		uint32_t m_width;
		uint32_t m_height;

		// virtual
		void Initialize();
	};

	struct D3D11Texture2D : public ITexture2D {
		D3D11GraphicsDevice* m_graphics_device;
		DXGI_FORMAT m_dxgi_format;
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11RenderTargetView> m_render_target_view;
		uint32_t m_width;
		uint32_t m_height;
	};

	struct D3D11SwapChain : public ISwapChain {
		HWND m_hwnd;
		DXGI_SWAP_CHAIN_DESC m_swap_desc = {};
		ComPtr<IDXGISwapChain> m_dxgi_swapchain;
		D3D11GraphicsDevice* m_graphics_device;
		D3D11Texture2D m_render_target_texture;
		D3D11ZStencil m_zstencil_buffer;

		// virtual
		void Initialize(uint32_t cx, uint32_t cy, uint32_t fps_num, uint32_t fps_den);
		void InitRenderTarget(uint32_t cx, uint32_t cy);
		void InitZStencilBuffer(uint32_t cx, uint32_t cy);
		void Resize(uint32_t cx, uint32_t cy);
		void SetDevice(IGraphicsDevice* graphics_device);

		void SetWindowHandle(HWND hwnd);
	};

	struct D3D11GraphicsDevice : public IGraphicsDevice {
		// virtual
		void Create(uint32_t adapter_index);
		void Destroy();
		void Initialize(uint32_t adapter_index);
		void SetRenderTarget(D3D11Texture2D* texture, D3D11ZStencil* zstencil);

		void InitFactory(uint32_t adapter_index);

		ComPtr<IDXGIFactory1> m_dxgi_factory;
		ComPtr<IDXGIAdapter1> m_adapter;
		ComPtr<ID3D11Device> m_d3d11_device;
		ComPtr<ID3D11DeviceContext> m_d3d11_context;
		D3D11_VIEWPORT m_viewport;

		D3D11SwapChain* m_swapchain;
		D3D11Texture2D* m_render_target;
		D3D11ZStencil* m_zstencil_buffer;
	};

};