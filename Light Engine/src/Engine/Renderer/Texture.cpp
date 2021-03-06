#include "ltpch.h"
#include "Texture.h"

#include "GraphicsContext.h"

#ifdef LIGHT_PLATFORM_WINDOWS
	#include "Platform/DirectX/dxTexture.h"
#endif
#include "Platform/Opengl/glTexture.h"

namespace Light {
	
	Texture::Texture(const std::string& atlasPath, const TextureCoordinates& texture, const TextureCoordinates& slice)
		: m_OccupiedSpace(texture), m_TextureUV(texture)
	{
		LT_PROFILE_FUNC();

		m_TextureUV.xMin *= 1.0f / slice.xMax;
		m_TextureUV.xMax *= 1.0f / slice.xMax;
		m_TextureUV.yMin *= 1.0f / slice.yMax;
		m_TextureUV.yMax *= 1.0f / slice.yMax;

		std::string atlas = FileManager::LoadTextFile(atlasPath);

		std::stringstream stream(atlas);
		std::string line;

		float xRatio = (texture.xMax - texture.xMin) / slice.xMax;
		float yRatio = (texture.yMax - texture.yMin) / slice.yMax;

		float xOffset = texture.xMin * xRatio;
		float yOffset = texture.yMin * yRatio;

		// note: I'm using (CodeAndWeb)TexturePacker with a custom exporter
		while (std::getline(stream, line))
		{
			std::istringstream lineStream(line);
			std::string temp;

			std::string name;
			float xMin, yMin, xMax, yMax;

			std::getline(lineStream, name, ' ');
			std::getline(lineStream, temp, ' '); xMin = std::stof(temp);
			std::getline(lineStream, temp, ' '); yMin = std::stof(temp);
			std::getline(lineStream, temp, ' '); xMax = std::stof(temp); xMax += xMin;
			std::getline(lineStream, temp, ' '); yMax = std::stof(temp); yMax += yMin;

			m_SubTextures[name] = { xMin * xRatio + xOffset,
			                        yMin * yRatio + yOffset,
			                        xMax * xRatio + xOffset,
			                        yMax * yRatio + yOffset,
			                        texture.sliceIndex };
		}
	}

	Texture::Texture(const TextureCoordinates& texture, const TextureCoordinates& slice)
		: m_OccupiedSpace(texture), m_TextureUV(texture)
	{
		m_TextureUV.xMin *= 1.0f / slice.xMax;
		m_TextureUV.xMax *= 1.0f / slice.xMax;
		m_TextureUV.yMin *= 1.0f / slice.yMax;
		m_TextureUV.yMax *= 1.0f / slice.yMax;
	}

	TextureArray::TextureArray(unsigned int width, unsigned int height, unsigned int depth, unsigned int channels)
		: m_Width(width), m_Height(height), m_Depth(depth), m_Channels(channels)
	{
	}

	std::shared_ptr<Light::TextureArray> TextureArray::Create(unsigned int width, unsigned int height, unsigned int depth, unsigned int channels /*= 4*/)
	{
		LT_PROFILE_FUNC();

		switch (GraphicsContext::GetAPI())
		{
		case GraphicsAPI::Opengl:
			return std::make_shared<glTextureArray>(width, height, depth, channels);
		case GraphicsAPI::Directx: LT_DX(
			return std::make_shared<dxTextureArray>(width, height, depth, channels);)
		default:
			LT_CORE_ASSERT(false, "TextureArray::Create: invalid GraphicsAPI");
		}
	}

	void TextureArray::LoadTextureAtlas(const std::string& name, const std::string& texturePath, const std::string& atlasPath)
	{
		LT_PROFILE_FUNC();
		m_UnresolvedTextures.push_back({ name, atlasPath, FileManager::LoadTextureFile(texturePath) });
	}

	void TextureArray::LoadTexture(const std::string& name, const std::string& texturePath)
	{
		LT_PROFILE_FUNC();
		m_UnresolvedTextures.push_back( { name, "", FileManager::LoadTextureFile(texturePath)} );
	}

	void TextureArray::AllocateTexture(const std::string& name, unsigned width, unsigned int height)
	{
		LT_PROFILE_FUNC();
		LT_CORE_ASSERT(std::max(width, height) <= LT_MAX_TEXTURE_DIMENSIONS,
		               "TextureArray::AllocateTexture: dimensions too large for texture: {}, {} > {}", name, std::max(width, height), LT_MAX_TEXTURE_DIMENSIONS);
		m_UnresolvedTextures.push_back({ name, "", TextureFileData(nullptr, width, height, m_Channels) });
	}

	void TextureArray::ResolveTextures()
	{
		LT_PROFILE_FUNC();
		std::sort(m_UnresolvedTextures.begin(), m_UnresolvedTextures.end(), std::greater());

		for (auto& data : m_UnresolvedTextures)
		{
			bool found = false;
			auto& t = data.texture;
		
			for (uint16_t z = 0; z < m_Depth && !found; z++)
			{
				found = false;
				for (uint16_t y = 0; y < m_Height - t.height && !found; y++)
				{
					for (uint16_t x = 0; x <= m_Width - t.width; x++)
					{
						const TextureCoordinates uv(x, y, x + t.width, y + t.height, z);
		
						bool valid = true;
						for (const auto& subt : m_OccupiedSpace)
							if(subt.Intersects(uv))
								{ valid = false; break; }
		
						if (valid)
						{
							found = true;
							m_OccupiedSpace.push_back(uv);
		
							if (t.pixels)
							{
								UpdateSubTexture(uv, t.pixels);
								t.Free();
							}
		
							if (!data.atlasPath.empty())
								m_Textures[data.name] = std::make_shared<Texture>(data.atlasPath, uv,
									                                              TextureCoordinates(0, 0, m_Width, m_Height, z));
							else
								m_Textures[data.name] = std::make_shared<Texture>(uv, TextureCoordinates(0, 0, m_Width, m_Height, z));
		
							break;
						}
					}
				}
			}
		
			LT_CORE_ASSERT(found, "TextureArray::ResolveTextures: could not find a valid space for texture");
			found = false;
		}
		
		m_UnresolvedTextures.clear();
		GenerateMips();
	}

	void TextureArray::DeleteTexture(const std::string& name)
	{
		LT_CORE_ASSERT(m_Textures.find(name) != m_Textures.end(), "TextureArray::DeleteTexture: failed to find texture: {}", name);

		auto it = std::find(m_OccupiedSpace.begin(), m_OccupiedSpace.end(), *m_Textures[name]->GetOccupiedSpace());
		LT_CORE_ASSERT(it != m_OccupiedSpace.end(), "TextureArray::DeleteTexture: occupied space of '{}' doesn't match any of TextureArray's", name);
		
		m_OccupiedSpace.erase(it);
		m_Textures.erase(name);
	}
	
}	