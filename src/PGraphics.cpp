#include "PGraphics.h"
#include "PLogger.h"

#include <exception>
#include <map>

#include <d3dcompiler.h>

// #pragma comment(lib, "dxgi.lib")
// #pragma comment(lib, "d3d11.lib")

namespace gfx {

const int32_t MAX_SCENES = 2048;
const float Z_FAR = 1000.f;
const float Z_NEAR = 1.f;

void render_clear() {
	auto device = graphics_core->m_gfx_device;
	auto ctx = device->m_d3d11_context;

	ID3D11RenderTargetView* rtv = device->m_swapchain->m_render_target.m_rtv.Get();
	const float clear_color[4] = {
		0.0, 1.0, 0.0, 1.0,
	};
	if (rtv && device) {
		ctx->ClearRenderTargetView(rtv, clear_color);

		uint32_t vert_stride = sizeof(Vertex);
		uint32_t vert_offset = 0;
		auto vert_buf = device->m_vertex_shader->m_vertex_buffer.GetAddressOf();
		ctx->IASetVertexBuffers(0, 1, vert_buf, &vert_stride, &vert_offset);
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		device->m_vertex_shader->SetParameters(
			device->m_model_matrix,
			device->m_view_matrix,
			device->m_proj_matrix);
		ctx->VSSetShader(device->m_vertex_shader->m_shader.Get(), nullptr, 0);
		ctx->PSSetShader(device->m_pixel_shader->m_shader.Get(), nullptr, 0);
		ctx->PSSetSamplers(0, 1, device->m_pixel_shader->m_sampler_state.GetAddressOf());
		ctx->Draw(6, 0);

		device->m_swapchain->m_dxgi_swapchain->Present(1, 0);
	}
}

void render_thread_main() {
	OutputDebugString(L"render_thread_main BEGIN\n");
	if (!graphics_core)
		return;

	while (!graphics_core->m_done) {
		// render...
		render_clear();
		// OutputDebugString(L"render_thread_main...\n");
	}

	OutputDebugString(L"render_thread_main END\n");
}

void reset_graphics(GraphicsDesc& gfx_desc, HWND view_wnd)
{
	ComPtr<ID3D11Buffer> back_buffer;
	GraphicsDevice *gfx_dev = nullptr;

	if (!graphics_core)
		graphics_core = std::unique_ptr<GraphicsCore>(new GraphicsCore);

	enumerate_adapters();

	if (!graphics_core->m_gfx_device)
		graphics_core->m_gfx_device = std::shared_ptr<GraphicsDevice>(
			new GraphicsDevice(gfx_desc.m_adapter_index));

	gfx_dev = graphics_core->m_gfx_device.get();
	if (!gfx_dev->m_swapchain)
		gfx_dev->m_swapchain = std::shared_ptr<Swapchain>(
			new Swapchain(graphics_core->m_gfx_device, gfx_desc, view_wnd));

	gfx_dev->m_swapchain->m_hwnd = view_wnd;
	gfx_dev->m_swapchain->m_gfx_device = graphics_core->m_gfx_device;
	gfx_dev->m_swapchain->m_gfx_desc = gfx_desc;
	gfx_dev->SetRenderTarget(
		gfx_dev->m_curr_render_target, gfx_dev->m_curr_zstencil_buf);

	bool shaders_ok = gfx_dev->InitShaders(
		L"hlsl\\solid.vs.hlsl", L"hlsl\\solid.ps.hlsl");
	if (!shaders_ok)
		PLogger::log(LOG_LEVEL::Error, L"Error in InitShaders!\n");

	gfx_dev->ResetViewport(
		gfx_desc.m_render_width, gfx_desc.m_render_height);

	gfx_dev->SetProjection(
		gfx_desc.m_render_width, gfx_desc.m_render_height, gfx_desc.m_orthographic);

	graphics_core->m_gfx_desc = gfx_desc;
	graphics_core->m_done = false;
	render_thread = std::thread(render_thread_main);
}

void shutdown_graphics()
{
	if (graphics_core)
		graphics_core->m_done = true;

	if (render_thread.joinable())
		render_thread.join();

	graphics_core->m_gfx_device->m_swapchain.reset();
	graphics_core->m_gfx_device.reset();
	graphics_core.reset(nullptr);

}

void ZBuffer::Init(bool enable) {
	D3D11_DEPTH_STENCIL_DESC stencil_desc;
	ComPtr<ID3D11DepthStencilState> stencil_state;
	stencil_desc.DepthEnable = enable;
	stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	stencil_desc.StencilEnable = true;
	stencil_desc.StencilReadMask = 0xFF;
	stencil_desc.StencilWriteMask = 0xFF;
	stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_gfx_device->m_d3d11_device->CreateDepthStencilState(&stencil_desc, stencil_state.GetAddressOf());
	m_gfx_device->m_d3d11_context->OMSetDepthStencilState(stencil_state.Get(), 1);
}

void Swapchain::InitRenderTarget(uint32_t cx, uint32_t cy) {
	HRESULT hr;

	m_render_target.m_gfx_device = m_gfx_device;
	m_render_target.m_width = cx;
	m_render_target.m_height = cy;
	m_render_target.m_dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM; //TODO: What situations call for using a different render target format?

	hr = m_dxgi_swapchain->GetBuffer(
		0, __uuidof(ID3D11Texture2D),
		(void**)m_render_target.m_d3d11_texture.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("IDXGISwapChain::GetBuffer ID3D11Texture2D failed for gfx_swapchain!\n");

	hr = m_gfx_device->m_d3d11_device->CreateRenderTargetView(
		m_render_target.m_d3d11_texture.Get(),
		NULL,
		m_render_target.m_rtv.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("ID3D11Device::CreateRenderTargetView failed for gfx_swapchain!\n");
}

void Swapchain::InitZStencilBuffer(bool enable, uint32_t cx, uint32_t cy)
{
	m_zstencil.m_gfx_device = m_gfx_device;
	m_zstencil.m_width = cx;
	m_zstencil.m_height = cy;
	m_zstencil.m_dxgi_format = DXGI_FORMAT_D24_UNORM_S8_UINT; //TODO: What situations call for using a zstencil different format?

	if (cx != 0 && cy != 0)
		m_zstencil.Init(enable);
}

void Swapchain::Init(uint32_t cx, uint32_t cy, bool z_enable) {
	InitRenderTarget(cx, cy);
	InitZStencilBuffer(z_enable, cx, cy);
}

void Swapchain::Resize(uint32_t cx, uint32_t cy, bool z_enable) {
	RECT client_rect;
	HRESULT hr;

	m_render_target.m_d3d11_texture.Reset();
	m_render_target.m_rtv.Reset();
	m_zstencil.m_d3d11_texture.Reset();
	m_zstencil.m_zs_view.Reset();

	if (cx == 0 || cy == 0) {
		::GetClientRect(m_hwnd, &client_rect);
		if (cx == 0) cx = client_rect.right;
		if (cy == 0) cy = client_rect.bottom;
	}

	hr = m_dxgi_swapchain->ResizeBuffers(1, cx, cy, m_render_target.m_dxgi_format, 0);
	if (FAILED(hr))
		throw std::exception("Failed to resize swap chain buffer!\n");

	InitRenderTarget(cx, cy);
	InitZStencilBuffer(z_enable, cx, cy);
}

Swapchain::Swapchain(std::shared_ptr<GraphicsDevice>& device, GraphicsDesc& gfx_desc, HWND hwnd)
:
m_gfx_device(device)
{
	HRESULT hr;
	m_hwnd = hwnd;

	// Create Swapchain Desc
	memset(&m_swap_desc, 0, sizeof(m_swap_desc));
	m_swap_desc.BufferCount = 1;
	m_swap_desc.BufferDesc.Format = gfx_desc.m_dxgi_format;
	/*m_swap_desc.BufferDesc.Width = gfx_desc.m_canvas_width;
	m_swap_desc.BufferDesc.Height = gfx_desc.m_canvas_height;*/
	m_swap_desc.BufferDesc.Width = gfx_desc.m_render_width;
	m_swap_desc.BufferDesc.Height = gfx_desc.m_render_height;
	m_swap_desc.BufferDesc.RefreshRate.Denominator = gfx_desc.m_fps_den;
	m_swap_desc.BufferDesc.RefreshRate.Numerator = gfx_desc.m_fps_num;
	m_swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_swap_desc.OutputWindow = hwnd;
	m_swap_desc.SampleDesc.Count = 1;
	m_swap_desc.Windowed = true;

	hr = m_gfx_device->m_dxgi_factory->CreateSwapChain(
		m_gfx_device->m_d3d11_device.Get(), &m_swap_desc, m_dxgi_swapchain.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("IDXGIFactory1::CreateSwapChain failed!\n");

	device->m_dxgi_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	//TODO: init texture for render target
	//Init(gfx_desc.m_canvas_width, gfx_desc.m_canvas_height);
	Init(gfx_desc.m_render_width, gfx_desc.m_render_height, gfx_desc.m_enable_zbuffer);
}

Swapchain::~Swapchain()
{
	m_dxgi_swapchain.Detach();
}

void enumerate_monitors(GraphicsAdapter& gfx_adapter, ComPtr<IDXGIAdapter1> dxgi_adapter)
{
	uint32_t idx = 0;
	ComPtr<IDXGIOutput> output;

	while(dxgi_adapter->EnumOutputs(idx++, &output) == S_OK) {
		DXGI_OUTPUT_DESC desc;
		if (FAILED(output->GetDesc(&desc)))
			continue;

		Monitor mon;
		mon.m_index = idx;
		mon.m_name = desc.DeviceName;
		mon.m_geometry.x = desc.DesktopCoordinates.left;
		mon.m_geometry.y = desc.DesktopCoordinates.top;
		mon.m_geometry.cx = desc.DesktopCoordinates.right;
		mon.m_geometry.cy = desc.DesktopCoordinates.bottom;
		mon.m_rotation = desc.Rotation;

		gfx_adapter.m_monitors.push_back(mon);

		PLogger::log(LOG_LEVEL::Debug, L"DX11 Monitor: %s\n", desc.DeviceName);
	}
}

//TODO: add adapter -> monitors to a std::map<adapter_name, monitors>
void enumerate_adapters() {
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGIAdapter1> dxgi_adapter;
	std::wstring name;
	uint32_t idx = 0;
	HRESULT hr;

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)factory.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

	while (factory->EnumAdapters1(idx++, dxgi_adapter.GetAddressOf()) == S_OK) {
		DXGI_ADAPTER_DESC desc;

		hr = dxgi_adapter->GetDesc(&desc);
		if (FAILED(hr))
			continue;

		PLogger::log(LOG_LEVEL::Debug, L"DXGI Adapter: %s\n", desc.Description);

		GraphicsAdapter adap;
		adap.m_index = idx;
		adap.m_name = desc.Description;

		enumerate_monitors(adap, dxgi_adapter);

		graphics_adapters.push_back(adap);
	}
}

GraphicsDevice::GraphicsDevice(uint32_t adapter_idx)
:
m_curr_render_target(nullptr),
m_curr_zstencil_buf(nullptr),
m_swapchain(nullptr),
m_vertex_shader(nullptr),
m_pixel_shader(nullptr)
{
	InitFactory(adapter_idx);
	InitDevice(adapter_idx);
	SetRasterState();
	SetRenderTarget(m_curr_render_target, m_curr_zstencil_buf);
}

GraphicsDevice::~GraphicsDevice()
{
	delete m_vertex_shader;
	delete m_pixel_shader;
	m_dxgi_factory.Detach();
	m_d3d11_context.Detach();
	m_d3d11_device.Detach();
	m_dxgi_adapter.Detach();
}

void GraphicsDevice::InitFactory(uint32_t adapter_idx) {
	HRESULT hr = 0;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)m_dxgi_factory.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw std::exception("CreateDXGIFactory(IDXGIFactory2) failed!");

	hr = m_dxgi_factory->EnumAdapters1(adapter_idx, &m_dxgi_adapter);
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

void GraphicsDevice::InitDevice(uint32_t adapter_idx) {
	std::wstring adapter_name;
	DXGI_ADAPTER_DESC desc;
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_10_0;
	bool nv12_supported = false;

	uint32_t create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#define DX11_DEBUG
#ifdef DX11_DEBUG
	create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = 0;

	hr = m_dxgi_adapter->GetDesc(&desc);
	if (hr == S_OK)
		adapter_name = desc.Description;
	else
		adapter_name = L"Unknown DXGI Adapter";

	hr = D3D11CreateDevice(
		m_dxgi_adapter.Get(),
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

bool GraphicsDevice::InitShaders(const wchar_t *vs_path, const wchar_t *ps_path) {
	bool res = false;

	m_vertex_shader = new VertexShader(std::make_shared<GraphicsDevice>(*this));
	m_pixel_shader = new PixelShader(std::make_shared<GraphicsDevice>(*this));

	// Rectangle verts
	Vertex vert[6];
	vert[0].m_position = { -100.0, -100.0, 0.0, 1.0 };
	vert[1].m_position = { -100.0, 100.0, 0.0, 1.0 };
	vert[2].m_position = { 100.0, -100.0, 0.0, 1.0 };
	vert[3].m_position = { 100.0, -100.0, 0.0, 1.0 };
	vert[4].m_position = { -100.0, 100.0, 0.0, 1.0 };
	vert[5].m_position = { 100.0, 100.0, 0.0, 1.0 };
	vert[0].m_tex_coord = { 0.0, 1.0 };
	vert[1].m_tex_coord = { 0.0, 0.0 };
	vert[2].m_tex_coord = { 1.0, 1.0 };
	vert[3].m_tex_coord = { 1.0, 1.0 };
	vert[4].m_tex_coord = { 0.0, 0.0 };
	vert[5].m_tex_coord = { 1.0, 0.0 };

	// Create Vertex Shader
	m_vertex_shader->CreateVertexBuffer(vert, sizeof(Vertex) * 6);
	m_vertex_shader->CompileVertexShader(vs_path);
	m_pixel_shader->CompilePixelShader(ps_path);

	if (m_vertex_shader->m_shader_blob && m_pixel_shader->m_shader_blob) {
		m_d3d11_device->CreateVertexShader(
			m_vertex_shader->m_shader_blob->GetBufferPointer(),
			m_vertex_shader->m_shader_blob->GetBufferSize(),
			nullptr,
			&m_vertex_shader->m_shader);

		m_vertex_shader->InitInputElements();
		m_vertex_shader->InitMatrixBuffer();

		// Create Pixel Shader
		m_d3d11_device->CreatePixelShader(
			m_pixel_shader->m_shader_blob->GetBufferPointer(),
			m_pixel_shader->m_shader_blob->GetBufferSize(),
			nullptr,
			&m_pixel_shader->m_shader);

		m_pixel_shader->InitSamplerState();

		res = true;
	}

	return res;
}

void GraphicsDevice::SetProjection(uint32_t width, uint32_t height, bool ortho)
{
	// Perspective projection
	if (!ortho) {
		float fov_y = (float)DirectX::XM_PI / 4.0f;
		float deg2rad = DirectX::XMConvertToRadians(45.0);
		float aspect = (float)width / (float)height;
		m_proj_matrix = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, Z_NEAR, Z_FAR);
	}
	// Orthographic projection
	else {
		m_proj_matrix = Matrix::CreateOrthographic(
			(float)width,
			(float)height,
			Z_NEAR,
			Z_FAR
		);
	}

	m_model_matrix = DirectX::XMMatrixIdentity();
	m_view_matrix = DirectX::XMMatrixIdentity();
}

void GraphicsDevice::SetRasterState()
{
	HRESULT hr;
	D3D11_RASTERIZER_DESC raster_desc;

	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_NONE;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = false;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	// create the rasterizer
	hr = m_d3d11_device->CreateRasterizerState(&raster_desc, m_d3d11_raster.GetAddressOf());

	m_d3d11_context->RSSetState(m_d3d11_raster.Get());
}

void GraphicsDevice::SetRenderTarget(struct Texture2d* texture, struct ZBuffer* zstencil) {
	if (m_swapchain) {
		if (!texture)
			texture = &this->m_swapchain->m_render_target;
		if (!zstencil)
			zstencil = &this->m_swapchain->m_zstencil;
	}

	if (this->m_curr_render_target == texture && this->m_curr_zstencil_buf == zstencil)
	if (m_curr_render_target == texture)
		return;

	if (texture && !texture->m_rtv)
		return;

	ID3D11RenderTargetView *rtv = texture->m_rtv.Get();
	this->m_curr_render_target = texture;
	this->m_curr_zstencil_buf = zstencil;
	this->m_d3d11_context->OMSetRenderTargets(1, &rtv, nullptr);
}

void GraphicsDevice::ResetViewport(uint32_t width, uint32_t height) {
	memset(&m_viewport, 0, sizeof(m_viewport));
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_d3d11_context->RSSetViewports(1, &m_viewport);
}

void VertexShader::CreateVertexBuffer(void *data, size_t elem_size, bool dynamic_usage) {
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

	hr = m_gfx_device->m_d3d11_device->CreateBuffer(&desc, &srd, &m_vertex_buffer);
	if (FAILED(hr))
		PLogger::log(LOG_LEVEL::Error, L"Error creating vertex buffer!\n");
	else
		// this->vertex_buffer = vbuf;
		PLogger::log(LOG_LEVEL::Info, L"Created Vertex Buffer\n");
}

HRESULT VertexShader::CompileVertexShader(const wchar_t *shader_path)
{
	HRESULT hr;
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined (DEBUG) || defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
#endif
	hr = D3DCompileFromFile(
		shader_path, nullptr, nullptr,
		"VSMain", "vs_4_0",
		flags, 0, &m_shader_blob, &m_error_blob);
	if (FAILED(hr))
		if (m_error_blob)
			PLogger::log(LOG_LEVEL::Error, (wchar_t *)m_error_blob->GetBufferPointer());
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			PLogger::log(LOG_LEVEL::Error, L"Error: Vertex shader path not found: %s\n", shader_path);

	return hr;
}

void VertexShader::InitInputElements()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    	//{"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = m_gfx_device->m_swapchain->m_gfx_device->m_d3d11_device->CreateInputLayout(layout, ARRAYSIZE(layout), m_shader_blob->GetBufferPointer(), m_shader_blob->GetBufferSize(), &m_vertex_layout);
	if (FAILED(hr))
		PLogger::log(LOG_LEVEL::Error, L"Error creating Vertex Shader input layout!\n");
	m_gfx_device->m_d3d11_context->IASetInputLayout(m_vertex_layout.Get());
}

void VertexShader::InitMatrixBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrix_desc;
	matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
	matrix_desc.ByteWidth = sizeof(MatrixBuffer);
	matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrix_desc.MiscFlags = 0;
	matrix_desc.StructureByteStride = 0;

	hr = m_gfx_device->m_d3d11_device->CreateBuffer(&matrix_desc, nullptr, &m_matrix_buffer);
	if (FAILED(hr))
		PLogger::log(LOG_LEVEL::Error, L"Error creating Vertex Shader Matrix Buffer!\n");
}

void VertexShader::SetParameters(struct Matrix& model, struct Matrix &view, struct Matrix &proj)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	MatrixBuffer *matrix_buf;

	model.Transpose();
	view.Transpose();
	proj.Transpose();

	auto ctx = m_gfx_device->m_d3d11_context;
	hr = ctx->Map(m_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		PLogger::log(LOG_LEVEL::Error, L"Failed to lock Matrix Buffer for writing!\n");
	matrix_buf = (MatrixBuffer *)mapped.pData;
	matrix_buf->m_model_matrix = model;
	matrix_buf->m_view_matrix = view;
	matrix_buf->m_proj_matrix = proj;

	ctx->Unmap(m_matrix_buffer.Get(), 0);

	ctx->VSSetConstantBuffers(0, 1, m_matrix_buffer.GetAddressOf());
}

HRESULT PixelShader::CompilePixelShader(const wchar_t *shader_path)
{
	HRESULT hr;
	uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined (DEBUG) || defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
#endif

	hr = D3DCompileFromFile(
		shader_path, nullptr, nullptr,
		"PSMain", "ps_4_0",
		flags, 0, &m_shader_blob, &m_error_blob);
	if (FAILED(hr))
		if (m_error_blob)
			PLogger::log(LOG_LEVEL::Error, (wchar_t *)m_error_blob->GetBufferPointer());
	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		PLogger::log(LOG_LEVEL::Error, L"Error: Pixel shader path not found: %s\n", shader_path);

	return hr;
}

void PixelShader::InitSamplerState()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	for (uint8_t b = 0; b < 4; b++)
		sampler_desc.BorderColor[b] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_gfx_device->m_swapchain->m_gfx_device->m_d3d11_device->CreateSamplerState(
		&sampler_desc, &m_sampler_state);
	if (FAILED(hr))
		PLogger::log(LOG_LEVEL::Error, L"Error creating Sampler State for pixel shader!\n");
}

};
