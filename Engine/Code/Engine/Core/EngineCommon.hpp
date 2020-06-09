#pragma once
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
/*#include "Engine/Input/InputSystem.hpp"*/

class InputSystem;

#define UNUSED(x) (void)(x);
typedef unsigned int uint;
typedef unsigned char byte;

const Vec2 ALIGN_BOTTOM_LEFT(0.f,0.f);
const Vec2 ALIGN_BOTTOM_RIGHT(1.f,0.f);
const Vec2 ALIGN_BOTTOM_CENTERED(0.5f,0.f);
const Vec2 ALIGN_CENTER_LEFT(0.f,0.5f);
const Vec2 ALIGN_CENTER_RIGHT(1.f,0.5f);
const Vec2 ALIGN_CENTERED(0.5f,0.5f);
const Vec2 ALIGN_TOP_LEFT(0.f,1.f);
const Vec2 ALIGN_TOP_RIGHT(1.f,1.f);
const Vec2 ALIGN_TOP_CENTERED(0.5f,1.f);

extern DevConsole* g_theConsole;
extern NamedStrings* g_gameConfigBlackboard;
extern EventSystem* g_theEventSystem;
extern InputSystem* g_theInput;


enum class eYawPitchRollRotationOrder
{
	YXZ,
	ZYX,
};

extern eYawPitchRollRotationOrder g_currentBases;