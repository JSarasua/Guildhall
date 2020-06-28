#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"



enum EntityFaction
{
	FACTION_INVALID = -1,

	FACTION_GOOD,
	FACTION_EVIL,
	FACTION_NEUTRAL,

	NUM_OF_FACTIONS
};


//Everything after ENTITY_TYPE_EXPLOSION will blend Additively
enum EntityType
{
	ENTITY_TYPE_INVALID = -1,

	ENTITY_TYPE_PLAYER,
	ENTITY_TYPE_NPC_ENEMY,
	//ENTITY_TYPE_NPC_TANK,
	//ENTITY_TYPE_BOULDER,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_EVIL_BULLET,
	ENTITY_TYPE_EXPLOSION,
	ENTITY_TYPE_LOOT,

	NUM_OF_ENTITY_TYPES
};


class Game;

class Entity
{
public:
	friend class Map;
	virtual ~Entity() {}
	Entity(Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity);

	virtual void Startup() = 0;
	virtual void Update(float deltaSeconds);
	virtual void Render() const = 0;

	bool IsFiring() const;
	void SetIsFiring( bool isFiring );

	bool IsGarbage() const;
	bool IsOffScreen();
	bool IsDodging() const;

	Vec2 GetForwardVector();
	float GetOrientationDegrees();
	float GetWeaponOrientationDegrees() const;
	Vec2 GetWeaponStartPosition();
	Vec2 GetBulletStartPosition() const;
	bool IsAlive() const;
	void SetAlive();
	const Vec2 GetPosition();
	virtual void Lose1Health();
	virtual void LoseHealth( int damage );
	virtual void LoseAllHealth();
	const Rgba8& GetColor();

	void SetPosition(const Vec2& newPosition);



protected:
	Vec2 m_position = Vec2(0.f,0.f);
	Vec2 m_velocity = Vec2(0.f,0.f);
	Vec2 m_weaponOffset = Vec2( 0.f, 0.f );
	Vec2 m_bulletOffset = Vec2( 0.f, 0.f );
	float m_orientationDegrees = 0.f;
	float m_weaponOrientationDegrees = 0.f;
	float m_angularVelocity = 1000.f;
	float m_physicsRadius = 1000.f;
	float m_cosmeticRadius = 2000.f;
	int m_health = 0;
	bool m_isDead = true;
	Rgba8 m_color = Rgba8(255,255,255);
	bool m_hasCollided = false;

	bool m_isGarbage = false;
	bool m_isFiring = false;

	AABB2 m_drawBounds;
	bool m_canWalk = false;
	bool m_canFly = false;
	bool m_canSwim = false;
	bool m_isDodging = false;

	EntityType m_entityType = ENTITY_TYPE_INVALID;
};