#pragma once
#include "Game/Entity.hpp"

class Game;


class Boulder : public Entity
{
public:

	Boulder( const Vec2& initialPosition, Game* game);
	Boulder( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game );
	~Boulder() {}



	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;


private:

};