#include "GraphicsD3D11.h"
#include "PLogger.h"

#include <string>

namespace pas {

	static std::unique_ptr<D3D11GraphicsCore> g_GraphicsCore;

	void device_clear()
	{
		auto dev = g_GraphicsCore->m_GraphicsDevice;
		ID3D11RenderTargetView* rtv = dev->m_Swapchain->m_RenderTargetTexture.m_RTV.Get();
		const float clear_color[4] = {
			1.0, 0.0, 0.0, 1.0,
		};

		if (rtv) {
			dev->m_D3D11Context->ClearRenderTargetView(rtv, clear_color);
			dev->m_Swapchain->m_DXGISwapChain->Present(1, 0);
		}
	}

	void video_thread_loop()
	{
		if (!g_GraphicsCore)
			return;
		while (!g_GraphicsCore->graphics_done) {
			device_clear();
		}
		OutputDebugString(L"video_thread_loop END\n");
	}

	void D3D11GraphicsCore::ResetVideo(D3D11GraphicsDesc& desc, HWND hwnd)
	{		
		if (!g_GraphicsCore)
			g_GraphicsCore = std::unique_ptr<D3D11GraphicsCore>(new D3D11GraphicsCore);
		if (!g_GraphicsCore->m_GraphicsDevice)
			g_GraphicsCore->m_GraphicsDevice = std::shared_ptr<D3D11GraphicsDevice>(new D3D11GraphicsDevice());

		g_GraphicsCore->m_GraphicsDevice->Create(desc.m_AdapterIndex);

		auto dev = g_GraphicsCore->m_GraphicsDevice;
		if (!dev->m_Swapchain)
			dev->m_Swapchain = std::shared_ptr<D3D11SwapChain>(new D3D11SwapChain(std::shared_ptr<D3D11GraphicsDevice>(dev)));
		
		dev->m_Swapchain->Initialize(desc, hwnd);
		dev->ResetViewport(desc.m_RenderWidth, desc.m_RenderHeight);

		g_GraphicsCore->graphics_done = false;
		video_thread = std::thread(video_thread_loop);
	}

	void D3D11GraphicsCore::Shutdown()
	{
		if (g_GraphicsCore)
			g_GraphicsCore->graphics_done = true;
		if (video_thread.joinable())
			video_thread.join();
		
		g_GraphicsCore->m_GraphicsDevice->Destroy();
	}

	void D3D11ZStencil::Initialize()
	{
		HRESULT hr;
		::ZeroMemory(&m_TextureDesc, sizeof(m_TextureDesc));
		m_TextureDesc.Width = m_Width;
		m_TextureDesc.Height = m_Height;
		m_TextureDesc.MipLevels = 1;
		m_TextureDesc.ArraySize = 1;
		m_TextureDesc.Format = m_DXGIFormat;
		m_TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_TextureDesc.SampleDesc.Count = 1;
		m_TextureDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = m_GraphicsDevice->m_D3D11Device->CreateTexture2D(
			&m_TextureDesc, NULL, m_Texture.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("Failed to create zstencil texture!\n");

		::ZeroMemory(&m_ZStencilViewDesc, sizeof(m_ZStencilViewDesc));
		m_ZStencilViewDesc.Format = m_DXGIFormat;
		m_ZStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = m_GraphicsDevice->m_D3D11Device->CreateDepthStencilView(
			m_Texture.Get(), &m_ZStencilViewDesc, m_ZStencilView.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("Failed to create zstencil view!\n");
	}

	void D3D11VertexBuffer::Create(void* data, size_t elem_size, bool dynamic_usage)
	{
		HRESULT hr;
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA srd;

		memset(&desc, 0, sizeof(desc));
		memset(&srd, 0, sizeof(srd));

		desc.Usage = dynamic_usage ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = dynamic_usage ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.ByteWidth = (UINT)elem_size;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.MiscFlags = 0;
		
		srd.pSysMem = data;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		hr = m_GraphicsDevice->m_D3D11Device->CreateBuffer(&desc, &srd, &m_VertexBuffer);
		if (FAILED(hr))
			PLogger::log(LOG_LEVEL::Error, L"Error creating vertex buffer!\n");
		else
			// this->vertex_buffer = vbuf;
			PLogger::log(LOG_LEVEL::Info, L"Created Vertex Buffer\n");
	}

	void D3D11SwapChain::Initialize(IGraphicsDesc& desc, HWND hwnd)
	{
		HRESULT hr;
		m_Hwnd = hwnd;

		memset(&m_SwapDesc, 0, sizeof(m_SwapDesc));
		m_SwapDesc.BufferCount = 1;
		m_SwapDesc.BufferDesc.Format = dynamic_cast<D3D11GraphicsDesc&>(desc).m_DXGIFormat;
		m_SwapDesc.BufferDesc.Width = desc.m_CanvasWidth;
		m_SwapDesc.BufferDesc.Height = desc.m_CanvasHeight;
		m_SwapDesc.BufferDesc.RefreshRate.Denominator = desc.m_FpsDen;
		m_SwapDesc.BufferDesc.RefreshRate.Numerator = desc.m_FpsNum;
		m_SwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_SwapDesc.OutputWindow = hwnd;
		m_SwapDesc.SampleDesc.Count = 1;
		m_SwapDesc.Windowed = true;

		hr = m_GraphicsDevice->m_DXGIFactory->CreateSwapChain(
			m_GraphicsDevice->m_D3D11Device.Get(), &m_SwapDesc, m_DXGISwapChain.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			throw std::exception("IDXGIFactory1::CreateSwapChain failed!\n");

		m_GraphicsDevice->m_DXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		InitRenderTarget(desc.m_CanvasWidth, desc.m_CanvasHeight);
		InitZStencilBuffer(desc.m_CanvasWidth, desc.m_CanvasHeight);
	}

	void D3D11SwapChain::InitRenderTarget(uint32_t cx, uint32_t cy)
	{
		HRESULT hr;
		m_RenderTargetTexture.m_GraphicsDevice = m_GraphicsDevice;
		m_RenderTargetTexture.m_Width = cx;
		m_RenderTargetTexture.m_Height = cy;
		m_RenderTargetTexture.m_DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		hr = m_DXGISwapChain->GetBuffer(
			0, __uuidof(ID3D11Texture2D),
			(void**)m_RenderTargetTexture.m_Texture.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("IDXGISwapChain::GetBuffer ID3D11Texture2D failed for gfx_swapchain!\n");

		hr = m_GraphicsDevice->m_D3D11Device->CreateRenderTargetView(
			m_RenderTargetTexture.m_Texture.Get(),
			NULL,
			m_RenderTargetTexture.m_RTV.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw std::exception("ID3D11Device::CreateRenderTargetView failed for gfx_swapchain!\n");
	}

	void D3D11SwapChain::InitZStencilBuffer(uint32_t cx, uint32_t cy)
	{
		m_ZStencilBuffer.m_GraphicsDevice = m_GraphicsDevice;
		m_ZStencilBuffer.m_Width = cx;
		m_ZStencilBuffer.m_Height = cy;
		m_ZStencilBuffer.m_DXGIFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (cx != 0 && cy != 0)
			m_ZStencilBuffer.Initialize();
	}

	void D3D11SwapChain::Resize(uint32_t cx, uint32_t cy)
	{
	}

	void D3D11SwapChain::SetWindowHandle(HWND hwnd)
	{
		m_Hwnd = hwnd;
	}

	const static D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

	D3D11GraphicsDevice::D3D11GraphicsDevice()
	:
	m_Swapchain(nullptr),
	m_CurrentRenderTarget(nullptr),
	m_CurrentZStencil(nullptr)
	{}

	D3D11GraphicsDevice::~D3D11GraphicsDevice()
	{}

	void D3D11GraphicsDevice::Create(uint32_t adapter_index)
	{
		InitFactory(adapter_index);
		Initialize(adapter_index);
		SetRenderTarget(nullptr, nullptr);
	}

	void D3D11GraphicsDevice::Destroy()
	{
		m_DXGIFactory.Detach();
		m_D3D11Context.Detach();
		m_D3D11Device.Detach();
		m_DXGIAdapter.Detach();
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

		hr = m_DXGIAdapter->GetDesc(&desc);
		if (hr == S_OK)
			adapter_name = desc.Description;

		hr = D3D11CreateDevice(
			m_DXGIAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			create_flags,
			feature_levels,
			sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION, m_D3D11Device.ReleaseAndGetAddressOf(),
			&feature_level,
			m_D3D11Context.ReleaseAndGetAddressOf()
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
		if (m_Swapchain) {
			if (!texture)
				texture = &m_Swapchain->m_RenderTargetTexture;
			if (!zstencil)
				zstencil = &m_Swapchain->m_ZStencilBuffer;
		}

		if (m_CurrentRenderTarget == texture && m_CurrentZStencil == zstencil)
			return;
		
		if (texture && !texture->m_RTV)
			return;

		ID3D11RenderTargetView* rtv = texture->m_RTV.Get();
		m_CurrentRenderTarget = texture;
		m_CurrentZStencil = zstencil;
		m_D3D11Context->OMSetRenderTargets(
			1, &rtv, zstencil ? zstencil->m_ZStencilView.Get() : nullptr);
	}

	void D3D11GraphicsDevice::InitFactory(uint32_t adapter_index)
	{
		HRESULT hr = 0;
		hr = CreateDXGIFactory(
			__uuidof(IDXGIFactory2),
			(void**)m_DXGIFactory.ReleaseAndGetAddressOf()
		);

		if (FAILED(hr))
			throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

		hr = m_DXGIFactory->EnumAdapters1(adapter_index, &m_DXGIAdapter);
		if (FAILED(hr))
			throw std::exception("IDXGIFactory2::EnumAdapters failed!");
	}

	void D3D11GraphicsDevice::ResetViewport(uint32_t width, uint32_t height)
	{
		memset(&m_Viewport, 0, sizeof(m_Viewport));
		m_Viewport.Width = (float)width;
		m_Viewport.Height = (float)height;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_D3D11Context->RSSetViewports(1, &m_Viewport);
	}
};