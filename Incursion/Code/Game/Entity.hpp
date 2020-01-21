#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


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
	ENTITY_TYPE_NPC_TURRET,
	ENTITY_TYPE_NPC_TANK,
	ENTITY_TYPE_BOULDER,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_EVIL_BULLET,
	ENTITY_TYPE_EXPLOSION,

	NUM_OF_ENTITY_TYPES
};





class Game;

class Entity
{
public:
	Entity() {}
	Entity(Game* game);
	Entity(EntityType type, EntityFaction faction, Game* game);
	~Entity() {}
	Entity(Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game);

	virtual void Startup() = 0;
	virtual void Update(float deltaSeconds);
	virtual void Render() const = 0;
	virtual void DebugRender() const;

	bool IsOffScreen();
	Vec2 GetForwardVelocityVector();
	Vec2 GetForwardVector() const;
	float GetOrientationDegrees() const;
	bool IsAlive();
	void SetAlive();
	const Vec2 GetPosition();
	virtual void Lose1Health();
	virtual void LoseAllHealth();
	const Rgba8& GetColor();

	bool IsPushedByEntities() const;
	bool IsPushedByWalls() const;
	bool DoesPushEntities() const;
	bool IsHitByBullets() const;

	void SetPosition(const Vec2& newPosition);
	void SetVelocity(const Vec2& newVelocity);


	EntityType m_EntityType = ENTITY_TYPE_INVALID;
	EntityFaction m_EntityFaction = FACTION_INVALID;

	float m_physicsRadius = 1000.f;
	float m_cosmeticRadius = 2000.f;

	void MarkGarbage();
	bool IsGarbage() const;



protected:
	Vec2 m_position = Vec2(0.f,0.f);
	Vec2 m_velocity = Vec2(0.f,0.f);
	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 0.f;
	Game* m_game = nullptr;

	int m_health = 0;
	bool m_isDead = true;
	bool m_isGarbage = false;
	Rgba8 m_color = Rgba8(255,255,255);


	bool m_isPushedByWalls = false;
	bool m_isPushedByEntities = false;
	bool m_doesPushEntities = false;
	bool m_isHitByBullets = false;




};


typedef std::vector<Entity*> EntityList;