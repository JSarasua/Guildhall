#pragma once
#include "Game/Entity.hpp"

class Game;



class Player : public Entity
{
public:

	Player( const Vec2& initialPosition, Game* game );
	Player( const Vec2& initialPosition, EntityType type, EntityFaction faction, Game* game );
	~Player() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

	Vec2 GetNosePosition() const;

	float GetCannonOrientation();
private:

	void UpdateFromJoystick( float deltaSeconds );
	void UpdateFromKeyboard( float deltaSeconds );


	void UpdatePlayerOrientationAndVelocity( float joystickOrientation, float joystickMagnitude, float deltaSeconds );
	void CapOrientationDegrees();
private:
	float m_cannonOrientation = 0.f;

};