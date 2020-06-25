#include "Game/Actor.hpp"

Actor::Actor( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	Entity( entityDef, initialPosition, pitchRollYawDegrees )
{

}

void Actor::Update( float deltaSeconds )
{

}

void Actor::Render() const
{

}

