#include "Engine/Core/EngineCommon.hpp"

DevConsole* g_theConsole				= nullptr;
NamedStrings* g_gameConfigBlackboard	= nullptr;
EventSystem* g_theEventSystem			= nullptr;
InputSystem* g_theInput					= nullptr;
NetworkSystem* g_theNetwork				= nullptr;
NetworkSys* g_theNetworkSys			= nullptr;

eYawPitchRollRotationOrder g_currentBases = eYawPitchRollRotationOrder::YXZ;