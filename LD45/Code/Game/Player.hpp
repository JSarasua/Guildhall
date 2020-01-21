#pragma once
#include "Game/Entity.hpp"

class Game;



class Player : public Entity
{
public:

	Player(const Vec2& initialPosition, int playerID, Game* game);
	~Player() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

	void ResetPosition();


private:

	void UpdateFromJoystick( float deltaSeconds );
	void UpdateFromKeyboard( float deltaSeconds );


private:

	Vec2 m_startingPosition;
	int m_playerID;
};