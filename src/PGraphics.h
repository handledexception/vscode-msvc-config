#pragma once

#include <cstdint>

#include <Windows.h>

#include <wrl/client.h> // ComPtr

#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>

using Microsoft::WRL::ComPtr;

typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_device gfx_device_t;

struct gfx_swapchain {
	HWND hwnd;
	DXGI_SWAP_CHAIN_DESC swap_desc;
	ComPtr<IDXGISwapChain> chain;
};

struct gfx_device {
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	uint32_t adapter_idx;

	void InitFactory(uint32_t adapter_idx);
	void InitDevice(uint32_t adapter_idx);

	gfx_device(uint32_t adapter_idx);
	~gfx_device();
};