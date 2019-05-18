#include <string>
#include "PGraphics.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

gfx_device::gfx_device(uint32_t adapter_idx)
{
	InitFactory(adapter_idx);
	InitDevice(adapter_idx);
}

void gfx_device::InitFactory(uint32_t adapter_idx)
{
	HRESULT hr = 0;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)factory.GetAddressOf());
	if (FAILED(hr)) {
		// throw exception
		return;
	}

	hr = factory->EnumAdapters1(adapter_idx, &adapter);
	if (FAILED(hr)) {
		// throw
		return;
	}
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
	DXGI_ADAPTER_DESC desc;
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_10_0;
	HRESULT hr = 0;
	uint32_t create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	std::wstring adapter_name;

	// create_flags |= D3D11_CREATE_DEVICE_DEBUG;

	hr = adapter->GetDesc(&desc);
	if (hr == S_OK) {
		adapter_name = desc.Description;
	}
	else {
		adapter_name = L"Unknown DXGI Adapter";
	}
	OutputDebugString(adapter_name.c_str());

	hr = D3D11CreateDevice(
		adapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		create_flags,
		feature_levels,
		sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION, device.GetAddressOf(),
		&feature_level,
		context.GetAddressOf()
	);
	if (FAILED(hr)) {
		// throw
		return;
	}
}