#pragma once
#include "Engine/Core/EngineCommon.hpp"
// #include "Engine/Math/vec2.hpp"
// #include "Engine/Math/Vec3.hpp"
// #include "Engine/Math/Vec4.hpp"
// #include "Engine/Math/IntVec2.hpp"
// #include "Engine/Core/Rgba8.hpp"
// #include "Engine/Math/AABB2.hpp"
// #include "Engine/Math/OBB2.hpp"
// #include "Engine/Math/Plane2.hpp"
// #include "Engine/Core/Vertex_PCU.hpp"
// #include "Engine/Core/Vertex_PCUTBN.hpp"
#include <vector>

struct Vec2;
struct Vec3;
struct Vec4;
struct IntVec2;
struct Rgba8;
struct AABB2;
struct OBB2;
struct Plane2;
struct Vertex_PCUTBN;
struct Vertex_PCU;

class BufferWriter
{
public:

	BufferWriter( std::vector<byte>& buffer, eBufferEndian endianMode = eBufferEndian::DEFAULTMODE );

	void AppendByte( byte dataToAppend );
	void AppendChar( char dataToAppend );
	void AppendBool( bool dataToAppend );
	void AppendUShort( unsigned short dataToAppend );
	void AppendShort( short dataToAppend );
	void AppendUInt32( unsigned int dataToAppend );
	void AppendInt32( int dataToAppend );
	void AppendFloat( float dataToAppend );
	void AppendUInt64( uint64_t dataToAppend );
	void AppendInt64( int64_t dataToAppend );
	void AppendDouble( double dataToAppend );

	void AppendStringZeroTerminated( std::string const& dataToAppend );
	void AppendStringAfter32BitLength( std::string const& dataToAppend );

	void AppendVec2( Vec2 const& dataToAppend );
	void AppendVec3( Vec3 const& dataToAppend );
	void AppendVec4( Vec4 const& dataToAppend );
	void AppendIntVec2( IntVec2 const& dataToAppend );
	void AppendRGBA8( Rgba8 const& dataToAppend );
	void AppendAABB2( AABB2 const& dataToAppend );
	void AppendOBB2( OBB2 const& dataToAppend );
	void AppendPlane2( Plane2 const& dataToAppend );
	void AppendVertexPCU( Vertex_PCU const& dataToAppend );
	void AppendVertexPCUTBN( Vertex_PCUTBN const& dataToAppend );

	void SetEndianMode( eBufferEndian bufferEndian );
	eBufferEndian GetEndianMode() const;

	size_t GetTotalSize() const { return m_buffer.size(); }
	size_t GetAppendedSize() const { return m_buffer.size(); }
	size_t GetCurrentLocation() const { return m_currentIndex; }

	void SetCurrentLocation( size_t newLocation ) { m_currentIndex = newLocation; }

	std::vector<byte> const& GetBuffer() const{ return m_buffer; }

private:
	std::vector<byte>& m_buffer;
	size_t m_currentIndex = 0;
	bool m_doesEndianMatch = true;
	eBufferEndian m_endianMode = DEFAULTMODE;
};