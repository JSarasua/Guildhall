#include "Engine/Core/EngineCommon.hpp"

DevConsole* g_theConsole				= nullptr;
NamedStrings* g_gameConfigBlackboard	= nullptr;
EventSystem* g_theEventSystem			= nullptr;
InputSystem* g_theInput					= nullptr;

eYawPitchRollRotationOrder g_currentBases = eYawPitchRollRotationOrder::YXZ;