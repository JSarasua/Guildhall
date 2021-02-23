#include "Engine/Core/BufferWriter.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

BufferWriter::BufferWriter( std::vector<byte>& buffer, eBufferEndian endianMode /*= eEndianMode::DEFAULTMODE */ ) :
	m_buffer( buffer )
{
	m_endianMode = endianMode;

	if( endianMode != eBufferEndian::DEFAULTMODE )
	{
		eBufferEndian hardwareEndianMode = GetHardwareEndianMode();
		m_doesEndianMatch = hardwareEndianMode == endianMode;
	}
}

void BufferWriter::AppendByte( byte dataToAppend )
{
	AppendDataToBuffer( &dataToAppend, sizeof( byte ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendChar( char dataToAppend )
{
	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( char ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendBool( bool dataToAppend )
{
	byte byteToAppend;
	if( dataToAppend )
	{
		byteToAppend = 1;
	}
	else
	{
		byteToAppend = 0;
	}
	AppendDataToBuffer( &byteToAppend, sizeof( byte ), m_buffer, m_currentIndex );

}

void BufferWriter::AppendUShort( unsigned short dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse2BytesInPlace( *(int16_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( unsigned short ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendShort( short dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse2BytesInPlace( *(int16_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( short ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendUInt32( unsigned int dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( unsigned int ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendInt32( int dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( int ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendFloat( float dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( float ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendUInt64( uint64_t dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( *(int64_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( uint64_t ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendInt64( int64_t dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( *(int64_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( int64_t ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendDouble( double dataToAppend )
{
	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( *(int64_t*)&dataToAppend );
	}

	AppendDataToBuffer( (byte*)&dataToAppend, sizeof( double ), m_buffer, m_currentIndex );
}

void BufferWriter::AppendStringZeroTerminated( std::string const& dataToAppend )
{
	AppendDataToBuffer( (byte*)&dataToAppend[0], dataToAppend.size(), m_buffer, m_currentIndex );
	byte zeroByte = 0;
	AppendDataToBuffer( &zeroByte, sizeof(char), m_buffer, m_currentIndex );
}

void BufferWriter::AppendStringAfter32BitLength( std::string const& dataToAppend )
{
	unsigned int stringLength = (unsigned int)dataToAppend.size();
	AppendUInt32( stringLength );

	AppendDataToBuffer( (byte*)&dataToAppend[0], dataToAppend.size(), m_buffer, m_currentIndex );

}

void BufferWriter::AppendVec2( Vec2 const& dataToAppend )
{
	AppendFloat( dataToAppend.x );
	AppendFloat( dataToAppend.y );
}

void BufferWriter::AppendVec3( Vec3 const& dataToAppend )
{
	AppendFloat( dataToAppend.x );
	AppendFloat( dataToAppend.y );
	AppendFloat( dataToAppend.z );
}

void BufferWriter::AppendVec4( Vec4 const& dataToAppend )
{
	AppendFloat( dataToAppend.x );
	AppendFloat( dataToAppend.y );
	AppendFloat( dataToAppend.z );
	AppendFloat( dataToAppend.w );
}

void BufferWriter::AppendIntVec2( IntVec2 const& dataToAppend )
{
	AppendInt32( dataToAppend.x );
	AppendInt32( dataToAppend.y );
}

void BufferWriter::AppendRGBA8( Rgba8 const& dataToAppend )
{
	AppendByte( dataToAppend.r );
	AppendByte( dataToAppend.g );
	AppendByte( dataToAppend.b );
	AppendByte( dataToAppend.a );
}

void BufferWriter::AppendAABB2( AABB2 const& dataToAppend )
{
	AppendVec2( dataToAppend.mins );
	AppendVec2( dataToAppend.maxs );
}

void BufferWriter::AppendOBB2( OBB2 const& dataToAppend )
{
	AppendVec2( dataToAppend.m_center );
	AppendVec2( dataToAppend.m_halfDimensions );
	AppendVec2( dataToAppend.m_iBasis );
}

void BufferWriter::AppendPlane2( Plane2 const& dataToAppend )
{
	AppendVec2( dataToAppend.fwdNormal );
	AppendFloat( dataToAppend.distance );
}

void BufferWriter::AppendVertexPCU( Vertex_PCU const& dataToAppend )
{
	AppendVec3( dataToAppend.position );
	AppendRGBA8( dataToAppend.tint );
	AppendVec2( dataToAppend.uvTexCoords );
}

void BufferWriter::AppendVertexPCUTBN( Vertex_PCUTBN const& dataToAppend )
{
	AppendVec3( dataToAppend.position );
	AppendRGBA8( dataToAppend.tint );
	AppendVec2( dataToAppend.uvTexCoords );
	AppendVec3( dataToAppend.tangent );
	AppendVec3( dataToAppend.bitangent );
}

void BufferWriter::SetEndianMode( eBufferEndian bufferEndian )
{
	m_endianMode = bufferEndian;
	
	if( bufferEndian != eBufferEndian::DEFAULTMODE )
	{
		eBufferEndian hardwareEndianMode = GetHardwareEndianMode();
		m_doesEndianMatch = hardwareEndianMode == bufferEndian;
	}
}

eBufferEndian BufferWriter::GetEndianMode() const
{
	return m_endianMode;
}

