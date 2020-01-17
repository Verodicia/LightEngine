#pragma once

#include "Renderer/Buffers.h"

#include "Core/Core.h"

#include <glm/glm.hpp>

namespace Light {
	
	enum UniformBufferIndex
	{
		UBufferIndex_ViewProjection = 0,
		// ...
	};

	class glUniformBuffers : public ConstantBuffers
	{
	private:
		unsigned int m_ViewProjBuffer = 0;
	public:
		glUniformBuffers();
		~glUniformBuffers();

		void SetViewMatrixImpl(void* view) override;
		void SetProjMatrixImpl(void* proj) override;
	};

	class glVertexBuffer : public VertexBuffer
	{
	private:
		unsigned int m_BufferID = 0;
	public:
		glVertexBuffer(float* vertices, unsigned int size);
		~glVertexBuffer();

		void Bind() override;

		void* Map  () override;
		void  UnMap() override;
	};

	class glIndexBuffer : public IndexBuffer
	{
	private:
		unsigned int m_BufferID = 0;
	public:
		glIndexBuffer(unsigned int* indices, unsigned int size);
		~glIndexBuffer();

		void Bind() override;
	};

}