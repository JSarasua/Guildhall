#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Core/EngineCommon.hpp"

RenderBuffer::RenderBuffer( RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memHint )
	: m_owner(owner),
	m_handle(nullptr),
	m_usage(usage),
	m_memHint(memHint),
	m_bufferByteSize(0U),
	m_elementByteSize(0U)
{
	//
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE( m_handle );
}

bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize ) const
{
	if( nullptr == m_handle )
	{
		return false;
	}

	if( m_elementByteSize != elementByteSize )
	{
		return false;
	}

	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else
	{
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::Cleanup()
{
	DX_SAFE_RELEASE( m_handle );
	m_bufferByteSize = 0;
	m_elementByteSize = 0;
}


bool RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementByteSize )
{
	// 1. If not compatible - destroy the old buffer
	// If we're GPU
		// buffersizes MUST match
	// if we're dyanamic
		// passed in buffer size is less than our bufferSize
	if( !IsCompatible( dataByteSize, elementByteSize ) )
	{
		Cleanup(); //destory the handle, reset things;

		// 2. If no buffer, create one that is compatible
		Create( dataByteSize, elementByteSize );
	}


	// 3. Updating the buffer
	ID3D11DeviceContext* context = m_owner->m_context;
	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
	// Mapping - Only available to DYNAMIC buffer
		D3D11_MAPPED_SUBRESOURCE mapped;

		// CPU -> GPU memory copy
		HRESULT result = context->Map( m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
		if( SUCCEEDED( result ) )
		{
			memcpy( mapped.pData, data, dataByteSize );
			context->Unmap(m_handle, 0 );
		}
		else
		{
			return false;
		}
	}
	else
	{
		// If this is MEMORY_HINT_GPU
		//Make sure this is compatible----------------------------------------------------
		context->UpdateSubresource( m_handle, 0, nullptr, data, 0, 0 );
	}
	// But, don't have to reallocate if going smaller

	// CopySubresource (direct copy)
	// This is only available to GPU buffers that have exactly the same size, and element size

	return true;
}

ID3D11Buffer* RenderBuffer::GetHandle()
{
	return m_handle;
}

UINT ToDXUsage( eRenderBufferUsage usage )
{
	UINT ret = 0;

	if( usage & VERTEX_BUFFER_BIT )
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if( usage & INDEX_BUFFER_BIT )
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if( usage & UNIFORM_BUFFER_BIT )
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

D3D11_USAGE ToDXMemoryUsage( eRenderMemoryHint hint )
{
	switch( hint )
	{
	case MEMORY_HINT_GPU: return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC: return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING: return D3D11_USAGE_STAGING;

	default: ERROR_AND_DIE( "Unknown hint" );
	}
}

bool RenderBuffer::Create( size_t dataByteSize, size_t elementByteSize )
{
	ID3D11Device* device = m_owner->m_device;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (uint)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );
	desc.CPUAccessFlags = 0;
	if( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if( m_memHint == MEMORY_HINT_STAGING )
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	desc.MiscFlags = 0; // used in special cases
	desc.StructureByteStride = (uint)elementByteSize;

	device->CreateBuffer( &desc, nullptr, &m_handle );

	if( nullptr != m_handle )
	{
		m_bufferByteSize = dataByteSize;
		m_elementByteSize = elementByteSize;
		return true;
	}

	return false;
}

void IndexBuffer::Update( uint indexCount, uint const* indices )
{
	RenderBuffer::Update( indices, indexCount * sizeof(uint), sizeof(uint));
}

void IndexBuffer::Update( std::vector<uint> const& indices )
{
	Update( (uint)indices.size(), &indices[0] );
}

VertexBuffer::VertexBuffer( RenderContext* context, eRenderMemoryHint memHint, uint stride, BufferAttribute const* layout )
	: RenderBuffer( context, VERTEX_BUFFER_BIT, memHint ),
	m_bufferAttribute( layout )
{
	m_elementByteSize = stride;
}
