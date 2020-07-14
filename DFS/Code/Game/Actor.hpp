#pragma once
#include <string>
#include "Game/Entity.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Time/Timer.hpp"
#include <vector>

class ActorDefinition;
class WeaponDefinition;
class BulletDefinition;

enum PlayerController
{
	Invalid_Player = -1,
	Player_1,
	Player_2,
	Player_3,
	Player_4,
	MaxNumPlayers
};

enum eBossAttacks
{
	InvalidAttack = -1,

	CircleOfBullets,
	RotatingLaser,
	RandomFire,

	MaxNumBossAttacks
};

enum eBossState
{
	InvalidState = -1,

	Moving,
	Attacking,

	MaxBossStates
};

class Actor : public Entity
{
	friend class Map;
public:
	Actor() = delete;
	Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition const* actorDef);
	Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition const* actorDef, PlayerController playerController);
	~Actor();

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

	void RenderWeapon() const;

	void SetEnemy( Entity* enemy );

	void IncrementActiveWeapon();
	void DecrementActiveWeapon();
	void AddWeapon( WeaponDefinition const* newWeapon );

	int GetBulletsPerShot() const;
	float GetBulletSpreadDegrees() const;
	Vec2 GetMuzzlePosition() const;

	BulletDefinition const* GetBulletDefinition() const;
	WeaponDefinition const* GetCurrentWeapon() const;

protected:
	void UpdateFromJoystick();
	void UpdateFromKeyboard();

protected:
	std::string m_name;
	ActorDefinition const* m_actorDef = nullptr;
	Vec2 m_goalPosition = Vec2(-1.f,-1.f);
	float m_timeSinceCreation = 0.f;
	float m_timeUntilNextGoalPosition = 2.f;
	Timer m_firingTimer;
	Timer m_dodgeTimer;
	Timer m_aiCooldownTimer;	//How long each burst of fire will be
	Timer m_aiStartupTimer;
	bool m_aiIsFiringState = false;
	PlayerController m_playerController = Invalid_Player;
	bool m_isWeaponFlipped = true;
	bool m_isWeaponInFront = true;
	Entity* m_enemyActor = nullptr;

	std::vector<WeaponDefinition const*> m_weapons;
	int m_currentWeaponIndex = 0;

	eBossAttacks m_currentBossAttack = InvalidAttack;
	eBossState m_currentBossState = InvalidState;
	int m_currentAttackIndex = 0;
	Timer m_bossWaitTimer;

private:
	void UpdateNPC( float deltaSeconds );
	void UpdateBoss( float deltaSeconds );
	void GetNewGoalPosition();
	std::string GetCurrentAnimationName() const;
};