#pragma once
#include <string>
#include "Game/Entity.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Time/Timer.hpp"

class ActorDefinition;

enum PlayerController
{
	Invalid_Player = -1,
	Player_1,
	Player_2,
	Player_3,
	Player_4,
	MaxNumPlayers
};

class Actor : public Entity
{
	friend class Map;
public:
	Actor() = delete;
	Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition* actorDef);
	Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition* actorDef, PlayerController playerController);
	~Actor(){}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

	void RenderWeapon() const;

	void SetEnemy( Entity* enemy );

protected:
	void UpdateFromJoystick();
	void UpdateFromKeyboard();

protected:
	std::string m_name;
	ActorDefinition* m_actorDef = nullptr;
	Vec2 m_goalPosition = Vec2(-1.f,-1.f);
	float m_timeSinceCreation = 0.f;
	float m_timeUntilNextGoalPosition = 2.f;
	Timer m_firingTimer;
	PlayerController m_playerController = Invalid_Player;
	bool m_isWeaponFlipped = true;
	bool m_isWeaponInFront = true;
	Entity* m_enemyActor = nullptr;

private:
	void UpdateNPC( float deltaSeconds );
	void GetNewGoalPosition();
	std::string GetCurrentAnimationName() const;
};