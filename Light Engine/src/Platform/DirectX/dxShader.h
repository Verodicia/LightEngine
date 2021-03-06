#pragma once

#include "Core/Core.h"

#include "Renderer/Shader.h"

#include <d3d11.h>
#include <wrl.h>

namespace Light {

	class dxShader : public Shader
	{
	private:
		static std::vector<std::pair<const std::string, const std::string>> s_TextureSlotsMap;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;	

		Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexBlob;
	public:
		dxShader(const std::string& vertexSource, std::string& pixelSource);
		~dxShader();

		void Bind() override;

		Microsoft::WRL::ComPtr<ID3D10Blob> GetVertexBlob() { return m_VertexBlob; }
	private:
		void ExtractVertexElements(const std::string& vertexSource);

		VertexElementType GetVertexElementType(const char* typeName);
	};

}