#include "Game/EntityFactory.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/BulletDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Game.hpp"

int EntityFactory::currentEntityID = 4;

Entity* EntityFactory::CreateEntity( CreateEntityMessage const& createMessage )
{
	int defIndex = createMessage.defIndex;
	int entityID = createMessage.entityID;
	int entityType = createMessage.entityType;
	float orientation = createMessage.initialOrientationDegrees;
	Vec2 position = createMessage.initialPosition;
	Vec2 velocity = createMessage.initialVelocity;
	float speedMultiplier = createMessage.speedMultiplier;

	Entity* entity = nullptr;

	if( entityType == ID_ACTOR )
	{
		ActorDefinition const* actorDef = ActorDefinition::GetActorDefFromIndex( defIndex );
		entity = EntityFactory::CreateActor( actorDef, position, velocity, orientation, 0.f, entityID );

		//This is a hack until weapons can be networked
		WeaponDefinition const* weaponDef = WeaponDefinition::GetRandomWeapon( g_theGame->m_rand );
		((Actor*)entity)->AddWeapon( weaponDef );
	}
	else if( entityType == ID_PLAYER )
	{
		ActorDefinition const* actorDef = ActorDefinition::GetActorDefFromIndex( defIndex );
		entity = EntityFactory::CreatePlayer( actorDef, position, velocity, orientation, 0.f, (PlayerController)entityID, entityID );
	}
	else if( entityType == ID_EVIL_BULLET )
	{
		BulletDefinition const* bulletDef = BulletDefinition::GetBulletDefFromIndex( defIndex );
		entity = EntityFactory::CreateBullet( bulletDef, position, orientation, ENTITY_TYPE_EVIL_BULLET, FACTION_EVIL, speedMultiplier, velocity, entityID );
	}
	else if( entityType == ID_GOOD_BULLET )
	{
		BulletDefinition const* bulletDef = BulletDefinition::GetBulletDefFromIndex( defIndex );
		entity = EntityFactory::CreateBullet( bulletDef, position, orientation, ENTITY_TYPE_GOOD_BULLET, FACTION_GOOD, speedMultiplier, velocity, entityID );

	}
	else if( entityType == ID_LOOT )
	{
		WeaponDefinition const* weaponDef = WeaponDefinition::GetWeaponDefFromIndex( defIndex );
		entity = EntityFactory::CreateLoot( position, weaponDef, entityID );
	}

	return entity;
}

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

	int actorDefIndex = ActorDefinition::GetActorDefIndex( actorDef );

	EventArgs args;
	args.SetValue( "position", initialPosition );
	args.SetValue( "velocity", initialVelocity );
	args.SetValue( "orientation", initialOrientationDegrees );
	args.SetValue( "speedMultiplier", 1.f );
	args.SetValue( "entityType", ID_ACTOR );
	args.SetValue( "defIndex", actorDefIndex );
	args.SetValue( "entityID", entityID );

	g_theEventSystem->FireEvent( "CreateEntity", NOCONSOLECOMMAND, &args );

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

	int actorDefIndex = ActorDefinition::GetActorDefIndex( actorDef );

	EventArgs args;
	args.SetValue( "position", initialPosition );
	args.SetValue( "velocity", initialVelocity );
	args.SetValue( "orientation", initialOrientationDegrees );
	args.SetValue( "speedMultiplier", 1.f );
	args.SetValue( "entityType", ID_ACTOR );
	args.SetValue( "defIndex", actorDefIndex );
	args.SetValue( "entityID", entityID );

	g_theEventSystem->FireEvent( "CreateEntity", NOCONSOLECOMMAND, &args );

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


	int actorDefIndex = ActorDefinition::GetActorDefIndex( actorDef );

	EventArgs args;
	args.SetValue( "position", initialPosition );
	args.SetValue( "velocity", initialVelocity );
	args.SetValue( "orientation", initialOrientationDegrees );
	args.SetValue( "speedMultiplier", 1.f );
	args.SetValue( "entityType", ID_PLAYER );
	args.SetValue( "defIndex", actorDefIndex );
	args.SetValue( "entityID", (int)playerController );

	g_theEventSystem->FireEvent( "CreateEntity", NOCONSOLECOMMAND, &args );

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

	int entityType = 0;
	if( type == ENTITY_TYPE_GOOD_BULLET )
	{
		entityType = ID_GOOD_BULLET;
	}
	else
	{
		entityType = ID_EVIL_BULLET;
	}
	int bulletDefIndex = BulletDefinition::GetBulletDefIndex( bulletDef );

	EventArgs args;
	args.SetValue( "position", initialPosition );
	args.SetValue( "velocity", entityVelocity );
	args.SetValue( "orientation", orientationDegrees );
	args.SetValue( "speedMultiplier", 1.f );
	args.SetValue( "entityType", entityType );
	args.SetValue( "defIndex", bulletDefIndex );
	args.SetValue( "entityID", entityID );

	g_theEventSystem->FireEvent( "CreateEntity", NOCONSOLECOMMAND, &args );

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

	int weaponDefIndex = WeaponDefinition::GetWeaponDefIndex( lootDef );

	EventArgs args;
	args.SetValue( "position", initialPosition );
	args.SetValue( "velocity", Vec2() );
	args.SetValue( "orientation", Vec2() );
	args.SetValue( "speedMultiplier", 1.f );
	args.SetValue( "entityType", ID_LOOT );
	args.SetValue( "defIndex", weaponDefIndex );
	args.SetValue( "entityID", entityID );

	g_theEventSystem->FireEvent( "CreateEntity", NOCONSOLECOMMAND, &args );

	return newLoot;
}

void EntityFactory::DeleteEntity( Entity* entityToDelete )
{
	if( entityToDelete )
	{
		int entityID = entityToDelete->m_entityID;
		EventArgs args;
		args.SetValue( "entityID", entityID );
		g_theEventSystem->FireEvent( "DeleteEntity", NOCONSOLECOMMAND, &args );

		delete entityToDelete;
	}

}

