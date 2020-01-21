#pragma once
#include "Game/Entity.hpp"

class Game;


class Bullet : public Entity
{
public:

	Bullet( const Vec2& initialPosition, Game* game);
	Bullet( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game );
	~Bullet() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;



private:


};