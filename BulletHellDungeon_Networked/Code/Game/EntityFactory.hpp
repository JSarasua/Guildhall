#pragma once
#include "Game/Entity.hpp"
#include "Game/Actor.hpp"
#include "Game/Bullet.hpp"
#include "Game/Loot.hpp"

class EntityFactory
{
public:
	static Actor* CreateActor( std::string actorDefStr, Vec2 initialPosition = Vec2(), Vec2 initialVelocity = Vec2(), float initialOrientationDegrees = 0.f, float initialAngularVelocity = 0.f, int entityID = -1 );
	static Actor* CreateActor( ActorDefinition const* actorDef, Vec2 initialPosition = Vec2(), Vec2 initialVelocity = Vec2(), float initialOrientationDegrees = 0.f, float initialAngularVelocity = 0.f, int entityID = -1 );
	static Actor* CreatePlayer( ActorDefinition const* actorDef, Vec2 initialPosition = Vec2(), Vec2 initialVelocity = Vec2(), float initialOrientationDegrees = 0.f, float initialAngularVelocity = 0.f, PlayerController playerController = Player_1, int entityID = -1 );
	static Bullet* CreateBullet( BulletDefinition const* bulletDef, Vec2 const& initialPosition = Vec2(), float orientationDegrees = 0.f, EntityType type = ENTITY_TYPE_GOOD_BULLET, EntityFaction faction = FACTION_GOOD, float speedMultiplier = 1.f, Vec2 const& entityVelocity = Vec2(), int entityID = -1 );
	static Loot* CreateLoot( Vec2 const& initialPosition, WeaponDefinition const* lootDef, int entityID = -1 );
	static void DeleteEntity( Entity* entityToDelete );

	static int currentEntityID;
};