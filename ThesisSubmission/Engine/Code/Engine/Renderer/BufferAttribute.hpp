#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_VEC2,
	BUFFER_FORMAT_VEC3,
	BUFFER_FORMAT_R8G8B8A8_UNORM,
};

struct BufferAttribute
{
public:
	std::string m_name; 			// used to link to a D3D11 shader
								// uint location; 			// used to link to a GL/Vulkan shader 
	eBufferFormatType m_type;		// what data are we describing
	uint m_offset; 				// where is it relative to the start of a vertex


public:
	BufferAttribute( char const* name, eBufferFormatType type, uint offset );
	BufferAttribute();
};