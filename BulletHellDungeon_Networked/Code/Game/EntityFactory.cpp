#include "Game/EntityFactory.hpp"
#include "Game/ActorDefinition.hpp"


Actor* EntityFactory::CreateActor( std::string actorDefStr, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f */ )
{
	ActorDefinition const* actorDef = nullptr;
	
	auto iter = ActorDefinition::s_definitions.find( actorDefStr );
	if( iter != ActorDefinition::s_definitions.end() )
	{
		actorDef = iter->second;
	}
	Actor* newActor = CreateActor( actorDef, initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity );

	return newActor;
}

Actor* EntityFactory::CreateActor( ActorDefinition const* actorDef, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f */ )
{
	Actor* newActor = new Actor( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity, actorDef );
	return newActor;
}

Actor* EntityFactory::CreatePlayer( ActorDefinition const* actorDef, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f*/, PlayerController playerController /*= Player_1 */ )
{

	if( playerController >= MaxNumPlayers )
	{
		return nullptr;
	}

	Actor* newActor = new Actor( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity, actorDef, playerController );
	return newActor;
}

Bullet* EntityFactory::CreateBullet( BulletDefinition const* bulletDef, Vec2 const& initialPosition /*= Vec2()*/, float orientationDegrees /*= 0.f*/, EntityType type /*= ENTITY_TYPE_GOOD_BULLET*/, EntityFaction faction /*= FACTION_GOOD*/, float speedMultiplier /*= 1.f*/, Vec2 const& entityVelocity /*= Vec2() */ )
{
	Bullet* newBullet = new Bullet( initialPosition, orientationDegrees, type, faction, bulletDef, speedMultiplier, entityVelocity );
	return newBullet;
}

Loot* EntityFactory::CreateLoot( Vec2 const& initialPosition, WeaponDefinition const* lootDef )
{
	Loot* newLoot = new Loot( initialPosition, lootDef );
	return newLoot;
}

void EntityFactory::DeleteEntity( Entity* entityToDelete )
{
	delete entityToDelete;
}

