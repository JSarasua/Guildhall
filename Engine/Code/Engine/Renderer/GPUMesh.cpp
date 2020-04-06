#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"


GPUMesh::GPUMesh( RenderContext* context )
{
	m_vertices = new VertexBuffer( context, MEMORY_HINT_DYNAMIC );
	m_indices = new IndexBuffer( context, MEMORY_HINT_DYNAMIC );
}

GPUMesh::~GPUMesh()
{
	delete m_vertices;
	m_vertices = nullptr;

 	delete m_indices;
 	m_indices = nullptr;
}

void GPUMesh::UpdateVertices( unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, BufferAttribute const* layout )
{
	UNUSED( vertexStride );
	UNUSED( layout );

	if( nullptr == layout )
	{
		m_vertices->m_bufferAttribute = Vertex_PCU::LAYOUT;
		m_vertexCount = vertexCount;
		Vertex_PCU* vertexes = (Vertex_PCU*)vertexData;
		size_t bufferTotalByteSize = vertexCount * sizeof( Vertex_PCU );
		size_t elementSize = sizeof( Vertex_PCU );
		m_vertices->Update( vertexes, bufferTotalByteSize, elementSize );
	}
	else
	{
		m_vertices->m_bufferAttribute = layout;
		if( layout == Vertex_PCU::LAYOUT )
		{
			m_vertexCount = vertexCount;
			Vertex_PCU* vertexes = (Vertex_PCU*)vertexData;
			size_t bufferTotalByteSize = vertexCount * sizeof( Vertex_PCU );
			size_t elementSize = sizeof( Vertex_PCU );
			m_vertices->Update( vertexes, bufferTotalByteSize, elementSize );
		}
		else if( layout == Vertex_PCUTBN::LAYOUT )
		{
			m_vertexCount = vertexCount;
			Vertex_PCUTBN* vertexes = (Vertex_PCUTBN*)vertexData;
			size_t bufferTotalByteSize = vertexCount * sizeof( Vertex_PCUTBN );
			size_t elementSize = sizeof( Vertex_PCUTBN );
			m_vertices->Update( vertexes, bufferTotalByteSize, elementSize );
		}
		else
		{
			ERROR_AND_DIE( "Invalid Layout" );
		}
	}
}

void GPUMesh::UpdateIndices( unsigned int indexCount, unsigned int const* indices )
{
	m_indexCount = indexCount;
	m_indices->Update( indexCount, indices );
}

void GPUMesh::UpdateIndices( std::vector<uint>& indices )
{
	UpdateIndices( (uint)indices.size(), &indices[0] );
}

VertexBuffer* GPUMesh::GetVertexBuffer() const
{
	return m_vertices;
}

IndexBuffer* GPUMesh::GetIndexBuffer() const
{
	return m_indices;
}

int GPUMesh::GetVertexCount() const
{
	return m_vertexCount;
}

int GPUMesh::GetIndexCount() const
{
	return m_indexCount;
}

