#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

struct BufferAttribute;
class RenderContext;
class VertexBuffer;
class IndexBuffer;

class GPUMesh
{
public:
	GPUMesh( RenderContext* context );
	~GPUMesh();

	void UpdateVertices( unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, BufferAttribute const* layout );
	void UpdateIndices( unsigned int indexCount, unsigned int const* indices );
	void UpdateIndices( std::vector<uint>& indices );


	VertexBuffer* GetVertexBuffer() const;
	IndexBuffer* GetIndexBuffer() const;

	int GetVertexCount() const;
	int GetIndexCount() const;

	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices( unsigned int vertexCount, VERTEX_TYPE const* vertices )
	{
		UpdateVertices( vertexCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT );
	}

	template <typename VERTEX_TYPE>
	void UpdateVertices( std::vector<VERTEX_TYPE>& vertices )
	{
		UpdateVertices( (unsigned int)vertices.size(), &vertices[0], sizeof( VERTEX_TYPE ), VERTEX_TYPE::LAYOUT );
	}


public:
	unsigned int m_vertexCount = 0;
	unsigned int m_indexCount = 0;
	VertexBuffer* m_vertices = nullptr;
	IndexBuffer* m_indices = nullptr;
};