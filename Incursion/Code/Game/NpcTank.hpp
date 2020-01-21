#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

class Game;

enum NpcTankState
{
	WANDERING,
	FINDING,
	PURSUING

};

enum NpcTankWallCollisionState
{
	NOTCOLLIDING,
	LEFTWHISKERCOLLIDING,
	RIGHTWHISKERCOLLIDING,
	BOTHWHISKERSCOLLIDING
};

class NpcTank : public Entity
{
public:

	NpcTank( const Vec2& initialPosition, Game* game );
	NpcTank( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game );
	~NpcTank() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	Vec2 GetNosePosition() const;

	bool m_DoesSeeEnemy = false;
	Vec2 m_EnemyPosition = Vec2( 0.f, 0.f );
	bool m_CanShootEnemy = false;
	bool m_CanDriveTowardsEnemy = false;
	bool m_UseMiddleWhisker = false;
	float m_leftWhisker = NPCTANK_WHISKERRANGE;
	float m_rightWhisker = NPCTANK_WHISKERRANGE;
	float m_middleWhisker = NPCTANK_WHISKERRANGE;
	NpcTankState m_currentState = WANDERING;
	NpcTankWallCollisionState m_wallCollisionState = NOTCOLLIDING;

private:


	void UpdateWander( float deltaSeconds );
	void UpdateFind( float deltaSeconds );
	void UpdatePursue( float deltaSeconds );
	float AvoidWalls( float goalOrientation );
	float SteerLeft( float goalOrientation );
	float SteerRight( float goalOrientation );


private:
	float m_velocityScale = 1.f; //Scaled velocity between 0 to 1 to avoid walls

};