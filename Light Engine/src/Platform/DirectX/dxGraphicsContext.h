#pragma once

#include "Renderer/GraphicsContext.h"

#include "Core/Core.h"

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

namespace Light {

	class Event;

	class WindowResizedEvent  ;
	class WindowMaximizedEvent;
	class WindowMinimizedEvent;
	class WindowRestoredEvent ;

	class dxGraphicsContext : public GraphicsContext
	{
	private:
		static dxGraphicsContext* s_Instance;
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	public:
		dxGraphicsContext(const GraphicsConfigurations& data);
		~dxGraphicsContext();

		void HandleWindowEvents(Event& event) override;

		void EnableVSync () override;
		void DisableVSync() override;

		void SwapBuffers() override;

		void Clear() override;
		void ClearBuffer(float r, float g, float b, float a) override;

		void Draw(unsigned int count) override;
		void DrawIndexed(unsigned int count) override;

		// Getters
		static ID3D11Device*        GetDevice       () { return s_Instance->m_Device.Get()       ; }
		static ID3D11DeviceContext* GetDeviceContext() { return s_Instance->m_DeviceContext.Get(); }
	private:
		bool OnWindowResize  (WindowResizedEvent&   event);
		bool OnWindowMaximize(WindowMaximizedEvent& event);
		bool OnWindowMinimize(WindowMinimizedEvent& event);
		bool OnWindowRestore (WindowRestoredEvent&  event);
	};

}