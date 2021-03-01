#pragma once
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/vec2.hpp"
#include <vector>

/*#include "Engine/Input/InputSystem.hpp"*/

class InputSystem;
class JobSystem;
class NetworkSystem;
class NetworkSys;
class UIManager;

#define UNUSED(x) (void)(x);
typedef unsigned int uint;
typedef unsigned char byte;
typedef std::vector<unsigned char> Buffer;

enum eBufferEndian
{
	INVALID = 0,
	LITTLE,
	BIG,
	DEFAULTMODE
};

eBufferEndian GetHardwareEndianMode();
void Reverse2BytesInPlace( int16_t& bytes );
void Reverse4BytesInPlace( int32_t& bytes );
void Reverse8BytesInPlace( int64_t& bytes );

const Vec2 ALIGN_BOTTOM_LEFT(0.f,0.f);
const Vec2 ALIGN_BOTTOM_RIGHT(1.f,0.f);
const Vec2 ALIGN_BOTTOM_CENTERED(0.5f,0.f);
const Vec2 ALIGN_CENTER_LEFT(0.f,0.5f);
const Vec2 ALIGN_CENTER_RIGHT(1.f,0.5f);
const Vec2 ALIGN_CENTERED(0.5f,0.5f);
const Vec2 ALIGN_TOP_LEFT(0.f,1.f);
const Vec2 ALIGN_TOP_RIGHT(1.f,1.f);
const Vec2 ALIGN_TOP_CENTERED(0.5f,1.f);

const Rgba8 CONSOLE_COLOR_INFORMATION_MAJOR = Rgba8::CYAN;
const Rgba8 CONSOLE_COLOR_INFORMATION_MINOR = Rgba8( 0, 128, 128, 255 );
const Rgba8 CONSOLE_COLOR_ERROR_MAJOR = Rgba8::RED;
const Rgba8 CONSOLE_COLOR_ERROR_MINOR = Rgba8( 128, 0, 0, 255 );

extern DevConsole* g_theConsole;
extern NamedStrings* g_gameConfigBlackboard;
extern EventSystem* g_theEventSystem;
extern InputSystem* g_theInput;
extern JobSystem* g_theJobSystem;
extern NetworkSystem* g_theNetwork;
extern NetworkSys* g_theNetworkSys;
extern UIManager* g_theUIManager;


enum class eYawPitchRollRotationOrder
{
	YXZ,
	ZYX,
};

extern eYawPitchRollRotationOrder g_currentBases;