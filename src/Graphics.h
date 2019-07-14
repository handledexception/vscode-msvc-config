#pragma once

#include <cstdint>
#include <memory>
#include <thread>

namespace pas {

	const int32_t MAX_SCENES = 2048;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;

	struct ISwapChain;
	struct ITexture2D;
	struct IZStencil;
	struct IGraphicsDevice;

	struct GraphicsDesc {
		uint32_t canvas_width;
		uint32_t canvas_height;
		uint32_t render_width;
		uint32_t render_height;
		uint32_t fps_num;
		uint32_t fps_den;
		uint32_t adapter_index;
	};

	struct GraphicsCore {
		IGraphicsDevice* m_graphics_device;
		GraphicsDesc m_graphics_desc;
		bool graphics_done;
		virtual void ResetVideo() = 0;
	};

	static GraphicsCore* graphics_core;

	struct ISwapChain {
		ISwapChain() {};
		virtual ~ISwapChain() {};
		virtual void Initialize(uint32_t cx, uint32_t cy, uint32_t fps_num, uint32_t fps_den) = 0;
		virtual void InitRenderTarget(uint32_t cx, uint32_t cy) = 0;
		virtual void InitZStencilBuffer(uint32_t cx, uint32_t cy) = 0;
		virtual void Resize(uint32_t cx, uint32_t cy) = 0;
		virtual void SetDevice(IGraphicsDevice* graphics_device) = 0;

		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_fps_num;
		uint32_t m_fps_den;
	};

	struct ITexture2D {
		ITexture2D() {};
		virtual ~ITexture2D() {};
	};

	struct IZStencil {
		IZStencil() {};
		virtual ~IZStencil() {};
		virtual void Initialize() = 0;
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

	struct IGraphicsDevice {
		IGraphicsDevice() {};
		virtual ~IGraphicsDevice() {};
		virtual void Create(uint32_t adapter_idx) = 0;
		virtual void Destroy() = 0;
		virtual void Initialize(uint32_t adapter_index) = 0;
		void SetRenderTarget(ITexture2D* texture, IZStencil* zstencil) {};
	};
};