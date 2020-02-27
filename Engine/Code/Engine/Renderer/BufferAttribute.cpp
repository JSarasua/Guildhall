#include "Engine/Renderer/BufferAttribute.hpp"




BufferAttribute::BufferAttribute( char const* name, eBufferFormatType type, uint offset )
{
// 	STATIC buffer_attribute_t const VertexPCU::LAYOUT[] ={
// 		buffer_attribute_t( "POSITION",  BUFER_FORMAT_VEC3,      		offsetof( VertexPCU, position ) ),
// 		buffer_attribute_t( "COLOR",     BUFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( VertexPCU, color ) ),
// 		buffer_attribute_t( "UV",        BUFER_FORMAT_VEC2,      		offsetof( VertexPCU, uv ) ),
// 		buffer_attribute_t() // end - terminator element; 
// 	};


	m_name = name;
	m_type = type;
	m_offset = offset;
}

BufferAttribute::BufferAttribute()
{

}

