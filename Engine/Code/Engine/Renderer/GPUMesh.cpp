#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"


void GPUMesh::UpdateVertices( unsigned int vertexCount, void const* vertexData, unsigned int vertexStride, BufferAttribute const* layout )
{
	UNUSED( vertexStride );
	UNUSED( layout );
	m_vertexCount = vertexCount;
	Vertex_PCU* vertexes = (Vertex_PCU*)vertexData;
	size_t bufferTotalByteSize = vertexCount * sizeof( Vertex_PCU );
	size_t elementSize = sizeof( Vertex_PCU );
	m_vertices->Update( vertexes, bufferTotalByteSize, elementSize );

}

