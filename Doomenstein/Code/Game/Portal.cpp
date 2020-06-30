#include "Game/Portal.hpp"
#include "Engine/Core/EngineCommon.hpp"

Portal::Portal( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	Entity( entityDef, initialPosition, pitchRollYawDegrees )
{

}

void Portal::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

