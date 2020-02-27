#pragma once

struct BufferAttribute;

class VertexBuffer;
class IndexBuffer;

class GPUMesh
{
public:
	void UpdateVertices( unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, BufferAttribute const* layout );
	void UpdateIndices( unsigned int indexCount, unsigned int const* indices );



	int GetIndexCount() const;

	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices( unsigned int vertexCount, VERTEX_TYPE const* vertices )
	{
		UpdateVertices( vertexCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT );
	}

public:
	unsigned int m_vertexCount = 0;
	VertexBuffer* m_vertices;
	IndexBuffer* m_indices;
};