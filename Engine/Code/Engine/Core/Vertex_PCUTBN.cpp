#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/BufferAttribute.hpp"

BufferAttribute const Vertex_PCUTBN::LAYOUT[] ={
	BufferAttribute( "POSITION",	BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCUTBN, position ) ),
	BufferAttribute( "COLOR",		BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCUTBN, tint ) ),
	BufferAttribute( "TEXCOORD",    BUFFER_FORMAT_VEC2,      		offsetof( Vertex_PCUTBN, uvTexCoords ) ),
	BufferAttribute( "NORMAL",		BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, normal ) ),
	BufferAttribute() // end - terminator element; 
};

Vertex_PCUTBN::Vertex_PCUTBN( Vertex_PCUTBN const& copyFrom )
	: position( copyFrom.position )
	, tint( copyFrom.tint )
	, uvTexCoords( copyFrom.uvTexCoords )
	, normal( copyFrom.normal )
{}

Vertex_PCUTBN::Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal )
	: position( position )
	, tint( tint )
	, uvTexCoords( uvTexCoords )
	, normal( normal )
{}

