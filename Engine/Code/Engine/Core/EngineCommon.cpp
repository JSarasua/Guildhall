#include "Engine/Core/EngineCommon.hpp"

DevConsole* g_theConsole				= nullptr;
NamedStrings* g_gameConfigBlackboard	= nullptr;
EventSystem* g_theEventSystem			= nullptr;
InputSystem* g_theInput					= nullptr;
NetworkSystem* g_theNetwork				= nullptr;
NetworkSys* g_theNetworkSys			= nullptr;

eYawPitchRollRotationOrder g_currentBases = eYawPitchRollRotationOrder::YXZ;

eBufferEndian GetHardwareEndianMode()
{
	int32_t anInteger = 12345678;

	byte firstByte = (byte)anInteger;
	if( firstByte == 12 )
	{
		return eBufferEndian::BIG;
	}
	else if( firstByte == 78 )
	{
		return eBufferEndian::LITTLE;
	}
	else
	{
		ERROR_RECOVERABLE( "Endian Mode isn't big or little" );
		return eBufferEndian::LITTLE;
	}
}

void Reverse2BytesInPlace( int16_t& bytes )
{
	bytes = ((bytes & 0x00ff) << 8) |
		((bytes & 0xff00) >> 8);
}

void Reverse4BytesInPlace( int32_t& bytes )
{
	bytes = ((bytes & 0x000000ff) << 24) |
		((bytes & 0x0000ff00) << 8)  |
		((bytes & 0x00ff0000) >> 8)  |
		((bytes & 0xff000000) >> 24);
}

void Reverse8BytesInPlace( int64_t& bytes )
{
	bytes = ((bytes & 0x00000000000000ff) << 56) |
		((bytes & 0x000000000000ff00) << 40) |
		((bytes & 0x0000000000ff0000) << 24) |
		((bytes & 0x00000000ff000000) << 8) |
		((bytes & 0x000000ff00000000) >> 8) |
		((bytes & 0x0000ff0000000000) >> 24) |
		((bytes & 0x00ff000000000000) >> 40) |
		((bytes & 0xff00000000000000) >> 56);
}