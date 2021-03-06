#pragma once

#include "Core/Core.h"

#include "Renderer/Framebuffer.h"

#include <d3d11.h>
#include <wrl.h>

namespace Light {

	class Shader;

	class dxFramebuffer : public Framebuffer
	{
	private:
		std::shared_ptr<Shader> m_Shader;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_TargetView;

		TextureBindingSlot m_BindingSlot;
	public:
		dxFramebuffer(const std::string& vertex, const std::string& pixel, unsigned int width, unsigned int height, TextureBindingSlot slot);
		~dxFramebuffer();

		void BindAsTarget() override;
		void BindAsResource() override;

		void Resize(unsigned int width, unsigned int height) override;
	};

}