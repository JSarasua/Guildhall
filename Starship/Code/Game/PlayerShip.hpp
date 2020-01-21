#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class PlayerShip : public Entity
{
public:
	PlayerShip(){}
	PlayerShip(Game* game);
	PlayerShip(Vec2 initialPosition, Game* game);
	~PlayerShip();

	virtual void Startup() override;

	virtual void Update(float deltaSeconds);
	virtual void Render() const override;

	virtual void Lose1Health() override;
	virtual void LoseAllHealth() override;


	float getOrientationDegrees();
	void ResurrectPlayer();
	Vec2 GetNosePosition();
	int GetLivesCount();

private:
	void IncreaseVelocity( float deltaSpeedUp );
	void UpdateFromJoystick( float deltaSeconds );
	void CheckCollidingWithWall();


private:
	int m_lives = 4;
};


