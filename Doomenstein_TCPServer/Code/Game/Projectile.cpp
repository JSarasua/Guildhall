#include "Game/Projectile.hpp"
#include "Engine/Core/EngineCommon.hpp"

Projectile::Projectile( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	Entity( entityDef, initialPosition, pitchRollYawDegrees )
{

}

void Projectile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Projectile::Render() const
{
	Entity::Render();
}

