#include "GraphicsD3D11.h"
#include "PLogger.h"

#include <string>

namespace pas {

	void D3D11ZStencil::Initialize()
	{
		HRESULT hr;
		::ZeroMemory(&m_texture_desc, sizeof(m_texture_desc));
		m_texture_desc.Width = m_width;
		m_texture_desc.Height = m_height;
		m_texture_desc.MipLevels = 1;
		m_texture_desc.ArraySize = 1;
		m_texture_desc.Format = m_dxgi_format;
		m_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_texture_desc.SampleDesc.Count = 1;
		m_texture_desc.Usage = D3D11_USAGE_DEFAULT;

		hr = m_graphics_device->m_d3d11_device->CreateTexture2D(
			&m_texture_desc, NULL, m_texture.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("Failed to create zstencil texture!\n");

		::ZeroMemory(&m_zstencil_view_desc, sizeof(m_zstencil_view_desc));
		m_zstencil_view_desc.Format = m_dxgi_format;
		m_zstencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = m_graphics_device->m_d3d11_device->CreateDepthStencilView(
			m_texture.Get(), &m_zstencil_view_desc, m_zstencil_view.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("Failed to create zstencil view!\n");
	}

	void D3D11SwapChain::Initialize(uint32_t cx, uint32_t cy, uint32_t fps_num, uint32_t fps_den)
	{
		m_width = cx;
		m_height = cy;
		m_fps_num = fps_num;
		m_fps_den = fps_den;
		InitRenderTarget(cx, cy);
		InitZStencilBuffer(cx, cy);
	}

	void D3D11SwapChain::InitRenderTarget(uint32_t cx, uint32_t cy)
	{
		HRESULT hr;
		m_render_target_texture.m_graphics_device = m_graphics_device;
		m_render_target_texture.m_width = cx;
		m_render_target_texture.m_height = cy;
		m_render_target_texture.m_dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;

		hr = m_dxgi_swapchain->GetBuffer(
			0, __uuidof(ID3D11Texture2D),
			(void**)m_render_target_texture.m_texture.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("IDXGISwapChain::GetBuffer ID3D11Texture2D failed for gfx_swapchain!\n");

		hr = m_graphics_device->m_d3d11_device->CreateRenderTargetView(
			m_render_target_texture.m_texture.Get(),
			NULL,
			m_render_target_texture.m_render_target_view.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("ID3D11Device::CreateRenderTargetView failed for gfx_swapchain!\n");
	}

	void D3D11SwapChain::InitZStencilBuffer(uint32_t cx, uint32_t cy)
	{
		m_zstencil_buffer.m_graphics_device = m_graphics_device;
		m_zstencil_buffer.m_width = cx;
		m_zstencil_buffer.m_height = cy;
		m_zstencil_buffer.m_dxgi_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (cx != 0 && cy != 0)
			m_zstencil_buffer.Initialize();
	}

	const static D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

	void D3D11GraphicsDevice::Create(uint32_t adapter_index)
	{
		InitFactory(adapter_index);
		Initialize(adapter_index);
		SetRenderTarget(m_render_target, m_zstencil_buffer);
	}

	void D3D11GraphicsDevice::Destroy()
	{
		m_dxgi_factory.Detach();
		m_d3d11_context.Detach();
		m_d3d11_device.Detach();
		m_adapter.Detach();
	}

	void D3D11GraphicsDevice::Initialize(uint32_t adapter_index)
	{
		HRESULT hr = 0;
		DXGI_ADAPTER_DESC desc = { 0 };
		D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_9_3;
		std::wstring adapter_name;
		bool nv12_supported = false;
		uint32_t create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#ifdef _DEBUG
			create_flags |= D3D11_CREATE_DEVICE_DEBUG;
			create_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE;
		#endif

		hr = m_adapter->GetDesc(&desc);
		if (hr == S_OK)
			adapter_name = desc.Description;

		hr = D3D11CreateDevice(
			m_adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			create_flags,
			feature_levels,
			sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION, m_d3d11_device.ReleaseAndGetAddressOf(),
			&feature_level,
			m_d3d11_context.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("D3D11CreateDevice failed!");

		PLogger::log(
			LOG_LEVEL::Debug,
			L"DX11 InitDevice: %s, Feature Level: %u\n",
			adapter_name.c_str(),
			(uint32_t)feature_level
		);

		//TODO: check for NV12 support
	}

	void D3D11GraphicsDevice::SetRenderTarget(D3D11Texture2D* texture, D3D11ZStencil* zstencil)
	{
		if (m_swapchain) {
			if (!texture)
				texture = &m_swapchain->m_render_target_texture;
			if (!zstencil)
				zstencil = &m_swapchain->m_zstencil_buffer;
		}

		if (m_render_target == texture && m_zstencil_buffer == zstencil)
			return;
		
		if (texture && !texture->m_render_target_view)
			return;

		ID3D11RenderTargetView* rtv = texture->m_render_target_view.Get();
		m_render_target = texture;
		m_zstencil_buffer = zstencil;
		m_d3d11_context->OMSetRenderTargets(1, &rtv, zstencil ? zstencil->m_zstencil_view.Get() : nullptr);
	}

	void D3D11GraphicsDevice::InitFactory(uint32_t adapter_index)
	{
		HRESULT hr = 0;
		hr = CreateDXGIFactory(
			__uuidof(IDXGIFactory2),
			(void**)m_dxgi_factory.ReleaseAndGetAddressOf()
		);

		if (FAILED(hr))
			throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

		hr = m_dxgi_factory->EnumAdapters1(adapter_index, &m_adapter);
		if (FAILED(hr))
			throw std::exception("IDXGIFactory2::EnumAdapters failed!");
		}
};