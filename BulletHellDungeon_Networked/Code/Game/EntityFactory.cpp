#include "Game/EntityFactory.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

int EntityFactory::currentEntityID = 4;

Actor* EntityFactory::CreateActor( std::string actorDefStr, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f */, int entityID )
{
	ActorDefinition const* actorDef = nullptr;
	
	auto iter = ActorDefinition::s_definitions.find( actorDefStr );
	if( iter != ActorDefinition::s_definitions.end() )
	{
		actorDef = iter->second;
	}
	Actor* newActor = CreateActor( actorDef, initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity );

	if( entityID == -1 )
	{
		currentEntityID++;
		newActor->m_entityID = currentEntityID;
	}
	else
	{
		newActor->m_entityID = entityID;
	}

	return newActor;
}

Actor* EntityFactory::CreateActor( ActorDefinition const* actorDef, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f */, int entityID )
{
	Actor* newActor = new Actor( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity, actorDef );
	
	if( entityID == -1 )
	{
		currentEntityID++;
		newActor->m_entityID = currentEntityID;
	}
	else
	{
		newActor->m_entityID = entityID;
	}

	return newActor;
}

Actor* EntityFactory::CreatePlayer( ActorDefinition const* actorDef, Vec2 initialPosition /*= Vec2()*/, Vec2 initialVelocity /*= Vec2()*/, float initialOrientationDegrees /*= 0.f*/, float initialAngularVelocity /*= 0.f*/, PlayerController playerController /*= Player_1 */, int entityID )
{
	UNUSED( entityID );
	if( playerController >= MaxNumPlayers )
	{
		return nullptr;
	}

	Actor* newActor = new Actor( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity, actorDef, playerController );
	newActor->m_entityID = playerController;
	return newActor;
}

Bullet* EntityFactory::CreateBullet( BulletDefinition const* bulletDef, Vec2 const& initialPosition /*= Vec2()*/, float orientationDegrees /*= 0.f*/, EntityType type /*= ENTITY_TYPE_GOOD_BULLET*/, EntityFaction faction /*= FACTION_GOOD*/, float speedMultiplier /*= 1.f*/, Vec2 const& entityVelocity /*= Vec2() */, int entityID )
{
	Bullet* newBullet = new Bullet( initialPosition, orientationDegrees, type, faction, bulletDef, speedMultiplier, entityVelocity );
	
	if( entityID == -1 )
	{
		currentEntityID++;
		newBullet->m_entityID = currentEntityID;
	}
	else
	{
		newBullet->m_entityID = entityID;
	}

	return newBullet;
}

Loot* EntityFactory::CreateLoot( Vec2 const& initialPosition, WeaponDefinition const* lootDef, int entityID )
{
	Loot* newLoot = new Loot( initialPosition, lootDef );

	if( entityID == -1 )
	{
		currentEntityID++;
		newLoot->m_entityID = currentEntityID;
	}
	else
	{
		newLoot->m_entityID = entityID;
	}

	return newLoot;
}

void EntityFactory::DeleteEntity( Entity* entityToDelete )
{
	delete entityToDelete;
}

