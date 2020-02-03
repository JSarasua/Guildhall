#pragma once

struct ID3D11Buffer;
class RenderContext;

typedef unsigned int uint;
typedef unsigned char byte;

#define BIT_FLAG(b)		(1 << (b))

//The "WHAT" are we using for
enum eRenderBufferUsageBit : uint
{
	VERTEX_BUFFER_BIT	= BIT_FLAG(0),	// A02 - Vertex Buffer (VBO)
	INDEX_BUFFER_BIT	= BIT_FLAG(1),	// A05 - Index Buffer (IBO)
	UNIFORM_BUFFER_BIT	= BIT_FLAG(2)	// A03 - Uniform Buffer (UBO)
};
typedef uint eRenderBufferUsage;

// The "HOW" - how are we going to access it
enum eRenderMemoryHint : uint
{
	MEMORY_HINT_GPU,		// GPU cna read/write, CPU can't touch it. If we change it, it changes rarely from CPU
	MEMORY_HINT_DYNAMIC,	// GPU memory (read/write) that changes OFTEN from the CPU - it allows to 'Map' the memory
	MEMORY_HINT_STAGING		// CPU read/write, and can copy from cpu-gpu
};

// A GPU Allocator (like malloc on the GPU, new byte[count])
class RenderBuffer
{
public:
	RenderBuffer( RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memHint );
	~RenderBuffer();
	//dataByteSize = totalBufferByteSize
	bool Update( void const* data, size_t dataByteSize, size_t elementByteSize );

private:
	bool Create( size_t dataByteSize, size_t elementByteSize );
	bool IsCompatible( size_t dataByteSize, size_t elementByteSize ) const;
	void Cleanup();

public:
	RenderContext* m_owner;
	ID3D11Buffer* m_handle;

	eRenderBufferUsage m_usage;
	eRenderMemoryHint m_memHint;

	size_t m_bufferByteSize;
	size_t m_elementByteSize;
};


class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer( RenderContext* context, eRenderMemoryHint memHint )
		: RenderBuffer( context, VERTEX_BUFFER_BIT, memHint )
	{}
};
