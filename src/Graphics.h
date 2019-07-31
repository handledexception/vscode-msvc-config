#pragma once

#define GFX_EXPORT __declspec( dllexport )

#include <cstdint>
#include <memory>
#include <thread>

#include <Windows.h>

namespace pas {

	const int32_t MAX_SCENES = 2048;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;

	struct ISwapChain;
	struct ITexture2D;
	struct IZStencil;
	struct GFX_EXPORT IGraphicsDevice;

	struct IGraphicsDesc {
		IGraphicsDesc() {};
		virtual ~IGraphicsDesc() {};
		uint32_t m_CanvasWidth;
		uint32_t m_CanvasHeight;
		uint32_t m_RenderWidth;
		uint32_t m_RenderHeight;
		uint32_t m_FpsNum;
		uint32_t m_FpsDen;
		uint32_t m_AdapterIndex;
	};

	struct GFX_EXPORT IGraphicsCore {
		IGraphicsCore() {};
		virtual ~IGraphicsCore() {};
		IGraphicsDesc m_graphics_desc;
		bool graphics_done;
	};

	struct ITexture2D {
		ITexture2D() {};
		virtual ~ITexture2D() {};
		
		uint32_t m_Width;
		uint32_t m_Height;
	};

	struct IZStencil {
		IZStencil() {};
		virtual ~IZStencil() {};
		virtual void Initialize() = 0;

		uint32_t m_Width;
		uint32_t m_Height;
	};

	// struct IPixelShader {
	// 	IPixelShader() {};
	// 	virtual IPixelShader() {};
	// };

	// struct IVertexShader {
	// 	IVertexShader() {};
	// 	virtual ~IVertexShader() {};
	// 	void CreateBuffer(ISwapChain* swap_chain);
	// };

	struct ISwapChain {
		// ISwapChain() {};
		virtual ~ISwapChain() {};
		virtual void Initialize(IGraphicsDesc& desc, HWND hwnd) = 0;
		virtual void InitRenderTarget(uint32_t cx, uint32_t cy) = 0;
		virtual void InitZStencilBuffer(uint32_t cx, uint32_t cy) = 0;
		virtual void Resize(uint32_t cx, uint32_t cy) = 0;
		//virtual void SetDevice(IGraphicsDevice* graphics_device) = 0;
		IGraphicsDesc m_GraphicsDesc;
	};

	struct GFX_EXPORT IGraphicsDevice {
		// IGraphicsDevice() {};
		virtual ~IGraphicsDevice() {};
		virtual void Create(uint32_t adapter_idx) = 0;
		virtual void Destroy() = 0;
		virtual void Initialize(uint32_t adapter_index) = 0;
		void SetRenderTarget(ITexture2D* texture, IZStencil* zstencil) {};
	};
};