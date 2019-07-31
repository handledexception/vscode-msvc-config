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
	static std::thread video_thread;

	struct D3D11SwapChain;
	struct GFX_EXPORT D3D11GraphicsDevice;
	struct D3D11Texture2D;
	struct D3D11ZStencil;
	struct D3D11GraphicsDesc;

	struct GFX_EXPORT D3D11GraphicsCore : public IGraphicsCore {
		static void ResetVideo(D3D11GraphicsDesc& desc, HWND hwnd);
		static void Shutdown();
		std::shared_ptr<D3D11GraphicsDevice> m_GraphicsDevice;
	};

	struct D3D11GraphicsDesc : public IGraphicsDesc {
		DXGI_FORMAT m_DXGIFormat;
	};

	struct D3D11ZStencil : public IZStencil {
		std::shared_ptr<D3D11GraphicsDevice> m_GraphicsDevice;
		DXGI_FORMAT m_DXGIFormat;
		ComPtr<ID3D11Texture2D> m_Texture;
		ComPtr<ID3D11DepthStencilView> m_ZStencilView;
		D3D11_TEXTURE2D_DESC m_TextureDesc = {};
		D3D11_DEPTH_STENCIL_VIEW_DESC m_ZStencilViewDesc = {};

		// virtual
		void Initialize();
	};

	struct D3D11Texture2D : public ITexture2D {
		std::shared_ptr<D3D11GraphicsDevice> m_GraphicsDevice;
		DXGI_FORMAT m_DXGIFormat;
		ComPtr<ID3D11Texture2D> m_Texture;
		ComPtr<ID3D11RenderTargetView> m_RTV;
	};

	struct D3D11VertexBuffer {
		D3D11VertexBuffer(std::shared_ptr<D3D11GraphicsDevice> device) : m_GraphicsDevice(device) {};
		void Create(void* data, size_t elem_size, bool dynamic_usage);
		ComPtr<ID3D11Buffer> m_VertexBuffer;
		std::shared_ptr<D3D11GraphicsDevice> m_GraphicsDevice;
	};

	struct D3D11VertexShader {
		
	};

	struct D3D11SwapChain : public ISwapChain {
		D3D11SwapChain(std::shared_ptr<D3D11GraphicsDevice> gfx_dev) : m_GraphicsDevice(gfx_dev) { };

		// virtual
		void Initialize(IGraphicsDesc& desc, HWND hwnd);
		void InitRenderTarget(uint32_t cx, uint32_t cy);
		void InitZStencilBuffer(uint32_t cx, uint32_t cy);
		void Resize(uint32_t cx, uint32_t cy);
		//void SetDevice(IGraphicsDevice* graphics_device);

		void SetWindowHandle(HWND hwnd);

		HWND m_Hwnd;
		DXGI_SWAP_CHAIN_DESC m_SwapDesc = {};
		ComPtr<IDXGISwapChain> m_DXGISwapChain;
		std::shared_ptr<D3D11GraphicsDevice> m_GraphicsDevice;
		D3D11Texture2D m_RenderTargetTexture;
		D3D11ZStencil m_ZStencilBuffer;
	};

	struct GFX_EXPORT D3D11GraphicsDevice : public IGraphicsDevice {
		D3D11GraphicsDevice();
		~D3D11GraphicsDevice();
		// virtual
		void Create(uint32_t adapter_index);
		void Destroy();
		void Initialize(uint32_t adapter_index);

		void SetRenderTarget(D3D11Texture2D* texture, D3D11ZStencil* zstencil);
		void InitFactory(uint32_t adapter_index);
		void ResetViewport(uint32_t width, uint32_t height);

		ComPtr<IDXGIFactory1> m_DXGIFactory;
		ComPtr<IDXGIAdapter1> m_DXGIAdapter;
		ComPtr<ID3D11Device> m_D3D11Device;
		ComPtr<ID3D11DeviceContext> m_D3D11Context;
		D3D11_VIEWPORT m_Viewport;

		std::shared_ptr<D3D11SwapChain> m_Swapchain;
		D3D11Texture2D* m_CurrentRenderTarget;
		D3D11ZStencil* m_CurrentZStencil;
	};

};