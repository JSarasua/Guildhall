#pragma once
#include "Game/Entity.hpp"

class Game;


class NpcTurret : public Entity
{
public:

	NpcTurret( const Vec2& initialPosition, Game* game );
	NpcTurret( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game );
	~NpcTurret() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	Vec2 GetNosePosition() const;
	bool m_DoesSeeEnemy = false;
	bool m_HasSeenEnemy = false;
	Vec2 m_EnemyPosition = Vec2(0.f, 0.f);
	bool m_CanShootEnemy = false;
	bool m_IsLookingForEnemy = false;

private:
	float m_currentGoalAngle = 0.f;
	float m_angleTowardsEnemy = 0.f;

	void SetNewGoalOrientation();
};
