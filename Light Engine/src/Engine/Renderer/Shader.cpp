#include "ltpch.h"
#include "Shader.h"

#include "GraphicsContext.h"

#include "Utility/FileManager.h"

#ifdef LIGHT_PLATFORM_WINDOWS
	#include "Platform/DirectX/dxShader.h"
#endif
#include "Platform/Opengl/glShader.h"

namespace Light {

	std::shared_ptr<Light::Shader> Shader::Create(const std::string& vertex, const std::string& fragment)
	{
		LT_PROFILE_FUNC();

		// figure out whether given string is the source or the path to the source
		bool vsSourceInFile = vertex.find('\n') == std::string::npos;
		bool fsSourceInFile = fragment.find('\n') == std::string::npos;

		// load shader source
		std::string vertex_source = vsSourceInFile ? FileManager::LoadTextFile(vertex) : vertex;
		std::string fragment_source = fsSourceInFile ? FileManager::LoadTextFile(fragment) : fragment;

		// assertions
		LT_CORE_ASSERT(!vertex_source.empty(), "Shader::Create: empty vertex shader source");
		LT_CORE_ASSERT(!fragment_source.empty(), "Shader::Create: empty fragment shader source");

		switch (GraphicsContext::GetAPI())
		{
		case GraphicsAPI::Opengl:
			ExtractShaderSource(vertex_source, "GLSL");
			ExtractShaderSource(fragment_source, "GLSL");

			return std::make_shared<glShader>(vertex_source, fragment_source);

		case GraphicsAPI::Directx: LT_DX(
			ExtractShaderSource(vertex_source, "HLSL");
			ExtractShaderSource(fragment_source, "HLSL");

			return std::make_shared<dxShader>(vertex_source, fragment_source); )

		default:
			LT_CORE_ASSERT(false, "Shader::Create: invalid GraphicsAPI");
		}
	}

	void Shader::ExtractShaderSource(std::string& src, const std::string& delim)
	{
		LT_PROFILE_FUNC();

		size_t begDelimPos, endDelimPos;
		
		begDelimPos = src.find("+" + delim) + 5;
		endDelimPos = src.find("-" + delim);

		LT_CORE_ASSERT(begDelimPos != std::string::npos + 5, 
		               "Shader::ExtractShaderSource: failed to find start delimiter in shader source, delim: {}, shader:\n{}",
		                delim, src);

		LT_CORE_ASSERT(endDelimPos != std::string::npos,
		               "Shader::ExtractShaderSource: failed to find end delimiter in shader source, delim: {}, shader:\n{}",
		                delim, src);

		src = src.substr(begDelimPos, endDelimPos - begDelimPos);
	}

}