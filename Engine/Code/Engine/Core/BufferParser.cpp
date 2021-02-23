#include "Engine/Core/BufferParser.hpp"

#include "Engine/Core/Vertex_PCUTBN.hpp"

BufferParser::BufferParser( byte const* bufferStart, size_t bufferSize, eBufferEndian endianMode ) :
	m_bufferStart( bufferStart ),
	m_bufferSize( bufferSize ),
	m_bufferEnd( bufferStart + bufferSize ),
	m_currentBufferPtr( m_bufferStart )
{
	m_endianMode = endianMode;

	if( endianMode != eBufferEndian::DEFAULTMODE )
	{
		eBufferEndian hardwareEndianMode = GetHardwareEndianMode();
		m_doesEndianMatch = hardwareEndianMode == endianMode;
	}
}

BufferParser::BufferParser( std::vector<byte> const& buffer, eBufferEndian endianMode )
{
	m_bufferStart = &buffer[0];
	m_currentBufferPtr = m_bufferStart;
	m_bufferSize = buffer.size();
	m_bufferEnd = m_bufferStart + m_bufferSize;
	m_endianMode = endianMode;

	if( endianMode != eBufferEndian::DEFAULTMODE )
	{
		eBufferEndian hardwareEndianMode = GetHardwareEndianMode();
		m_doesEndianMatch = hardwareEndianMode == endianMode;
	}
}

byte BufferParser::ParseByte()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( byte ) <= m_bufferEnd, "Tried to parse byte past end of buffer" );

	byte parsedByte = *(byte*)m_currentBufferPtr;
	m_currentBufferPtr++;

	return parsedByte;
}

bool BufferParser::ParseBool()
{
	byte parsedByte = ParseByte();

	if( parsedByte == 0 )
	{
		return false;
	}
	else if( parsedByte == 1 )
	{
		return true;
	}
	else
	{
		ERROR_AND_DIE( "Tried to parse a bool did not return a 0 or 1" );
	}
}

char BufferParser::ParseChar()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( char ) <= m_bufferEnd, "Tried to parse char past end of buffer" );

	char parsedChar = *(char*)m_currentBufferPtr;
	m_currentBufferPtr++;

	return parsedChar;
}

float BufferParser::ParseFloat()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( float ) <= m_bufferEnd, "Tried to parse float past end of buffer" );


	float parsedFloat = *(float*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( float );

	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&parsedFloat );
	}

	return parsedFloat;
}

int BufferParser::ParseInt32()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( int ) <= m_bufferEnd, "Tried to parse int past end of buffer" );


	int parsedInt = *(int*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( int );

	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&parsedInt );
	}

	return parsedInt;
}

unsigned int BufferParser::ParseUInt32()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( unsigned int ) <= m_bufferEnd, "Tried to parse unsigned int past end of buffer" );


	unsigned int parsedUInt = *(unsigned int*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( unsigned int );

	if( !m_doesEndianMatch )
	{
		Reverse4BytesInPlace( *(int32_t*)&parsedUInt );
	}

	return parsedUInt;
}

double BufferParser::ParseDouble()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( double ) <= m_bufferEnd, "Tried to parse double past end of buffer" );

	double parsedDouble = *(double*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( double );

	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( *(int64_t*)&parsedDouble );
	}
	
	return parsedDouble;
}

unsigned short BufferParser::ParseUShort()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( unsigned short ) <= m_bufferEnd, "Tried to parse unsigned short past end of buffer" );

	unsigned short parsedUShort = *(unsigned short*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( unsigned short );

	if( !m_doesEndianMatch )
	{
		Reverse2BytesInPlace( *(int16_t*)&parsedUShort );
	}

	return parsedUShort;
}

uint64_t BufferParser::ParseUInt64()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( uint64_t ) <= m_bufferEnd, "Tried to parse uint64_t past end of buffer" );

	uint64_t parsedU64 = *(uint64_t*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( uint64_t );

	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( *(int64_t*)&parsedU64 );
	}

	return parsedU64;
}

int64_t BufferParser::ParseInt64()
{
	GUARANTEE_OR_DIE( m_currentBufferPtr + sizeof( int64_t ) <= m_bufferEnd, "Tried to parse int64_t past end of buffer" );

	int64_t parsedInt64 = *(int64_t*)m_currentBufferPtr;
	m_currentBufferPtr += sizeof( int64_t );

	if( !m_doesEndianMatch )
	{
		Reverse8BytesInPlace( parsedInt64 );
	}

	return parsedInt64;
}

std::string BufferParser::ParseStringZeroTerminated()
{
	size_t stringLength = 0;
	char const* currentPointer = (char const*)m_currentBufferPtr;
	while( *currentPointer != 0 )
	{
		stringLength++;
		currentPointer++;
	}
	
	GUARANTEE_OR_DIE( m_currentBufferPtr + stringLength <= m_bufferEnd, "Tried to parse string past end of buffer" );

	std::string parsedString = std::string( (char const* const)m_currentBufferPtr, stringLength );
	m_currentBufferPtr += stringLength + 1;
	
	return parsedString;
}

std::string BufferParser::ParseStringAfter32BitLength()
{
	size_t stringLength = (size_t)ParseUInt32();

	GUARANTEE_OR_DIE( m_currentBufferPtr + stringLength <= m_bufferEnd, "Tried to parse string past end of buffer" );

	std::string parsedString = std::string( (char const* const)m_currentBufferPtr, stringLength );
	m_currentBufferPtr += stringLength;

	return parsedString;
}

Vec2 BufferParser::ParseVec2()
{
	Vec2 parsedVec2;
	parsedVec2.x = ParseFloat();
	parsedVec2.y = ParseFloat();

	return parsedVec2;
}

Vec3 BufferParser::ParseVec3()
{
	Vec3 parsedVec3;
	parsedVec3.x = ParseFloat();
	parsedVec3.y = ParseFloat();
	parsedVec3.z = ParseFloat();

	return parsedVec3;
}

Vec4 BufferParser::ParseVec4()
{
	Vec4 parsedVec4;
	parsedVec4.x = ParseFloat();
	parsedVec4.y = ParseFloat();
	parsedVec4.z = ParseFloat();
	parsedVec4.w = ParseFloat();

	return parsedVec4;
}

IntVec2 BufferParser::ParseIntVec2()
{
	IntVec2 parsedIntVec2;
	parsedIntVec2.x = ParseInt32();
	parsedIntVec2.y = ParseInt32();

	return parsedIntVec2;
}

Rgba8 BufferParser::ParseRGBA8()
{
	Rgba8 parsedRGBA8;
	parsedRGBA8.r = ParseByte();
	parsedRGBA8.g = ParseByte();
	parsedRGBA8.b = ParseByte();
	parsedRGBA8.a = ParseByte();

	return parsedRGBA8;
}

AABB2 BufferParser::ParseAABB2()
{
	AABB2 parsedAABB2;
	parsedAABB2.mins = ParseVec2();
	parsedAABB2.maxs = ParseVec2();

	return parsedAABB2;
}

OBB2 BufferParser::ParseOBB2()
{
	OBB2 parsedOBB2;
	parsedOBB2.m_center = ParseVec2();
	parsedOBB2.m_halfDimensions = ParseVec2();
	parsedOBB2.m_iBasis = ParseVec2();

	return parsedOBB2;
}

Plane2 BufferParser::ParsePlane2()
{
	Plane2 parsedPlane2;
	parsedPlane2.fwdNormal = ParseVec2();
	parsedPlane2.distance = ParseFloat();

	return parsedPlane2;
}

Vertex_PCU BufferParser::ParseVertexPCU()
{
	Vertex_PCU parsedVertexPCU;
	parsedVertexPCU.position = ParseVec3();
	parsedVertexPCU.tint = ParseRGBA8();
	parsedVertexPCU.uvTexCoords = ParseVec2();

	return parsedVertexPCU;
}

Vertex_PCUTBN BufferParser::ParseVertexPCUTBN()
{
	Vertex_PCUTBN parsedVertexPCUTBN;
	parsedVertexPCUTBN.position = ParseVec3();
	parsedVertexPCUTBN.tint = ParseRGBA8();
	parsedVertexPCUTBN.uvTexCoords = ParseVec2();
	parsedVertexPCUTBN.tangent = ParseVec3();
	parsedVertexPCUTBN.bitangent = ParseVec3();
	
	return parsedVertexPCUTBN;
}

void BufferParser::SetEndianMode( eBufferEndian bufferEndian )
{
	if( bufferEndian != eBufferEndian::DEFAULTMODE )
	{
		eBufferEndian hardwareEndianMode = GetHardwareEndianMode();
		m_doesEndianMatch = hardwareEndianMode == bufferEndian;
		m_endianMode = bufferEndian;
	}
}

eBufferEndian BufferParser::GetEndianMode() const
{
	return m_endianMode;
}

size_t BufferParser::GetRemainingSize() const
{
	size_t remainingSize = m_bufferEnd - m_currentBufferPtr;
	return remainingSize; 
}

void BufferParser::JumpToLocation( size_t offsetFromStart )
{
	m_currentBufferPtr = m_bufferStart + offsetFromStart;

	GUARANTEE_OR_DIE( m_currentBufferPtr <= m_bufferEnd, "Jumped beyond end of buffer" );
}

void BufferParser::JumpForward( size_t offsetFromCurrent )
{
	m_currentBufferPtr += offsetFromCurrent;

	GUARANTEE_OR_DIE( m_currentBufferPtr <= m_bufferEnd, "Jumped beyond end of buffer" );
}

