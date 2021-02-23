#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
//#include "Engine/Core/Vertex_PCUTBN.hpp"
struct Vertex_PCUTBN;

class BufferParser
{
public:

	BufferParser( byte const* bufferStart, size_t bufferSize, eBufferEndian endianMode = DEFAULTMODE );
	BufferParser( std::vector<byte> const& buffer, eBufferEndian endianMode = DEFAULTMODE );
	~BufferParser() {}

	byte ParseByte();
	bool ParseBool();
	char ParseChar();
	float ParseFloat();
	int ParseInt32();
	unsigned int ParseUInt32();
	double ParseDouble();
	unsigned short ParseUShort();
	uint64_t ParseUInt64();
	int64_t ParseInt64();

	std::string ParseStringZeroTerminated();
	std::string ParseStringAfter32BitLength();

	Vec2			ParseVec2();
	Vec3			ParseVec3();
	Vec4			ParseVec4();
	IntVec2			ParseIntVec2();
	Rgba8			ParseRGBA8();
	AABB2			ParseAABB2();
	OBB2			ParseOBB2();
	Plane2			ParsePlane2();
	Vertex_PCU		ParseVertexPCU();
	Vertex_PCUTBN	ParseVertexPCUTBN();

	void SetEndianMode( eBufferEndian bufferEndian );
	eBufferEndian GetEndianMode() const;

	size_t GetRemainingSize() const;
	void JumpToLocation( size_t offsetFromStart );
	void JumpForward( size_t offsetFromCurrent );
	size_t GetCurrentOffset() const { return m_currentBufferPtr - m_bufferStart; }

private:
	byte const* m_bufferStart = nullptr;
	byte const* m_bufferEnd = nullptr;
	byte const* m_currentBufferPtr = nullptr;
	size_t m_bufferSize = 0;
	
	bool m_doesEndianMatch = true;
	eBufferEndian m_endianMode = DEFAULTMODE;
};