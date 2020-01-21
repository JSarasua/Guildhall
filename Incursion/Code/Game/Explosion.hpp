#pragma once
#include "Game/Entity.hpp"

class Game;
class SpriteAnimDefinition;

class Explosion : public Entity
{
public:
	Explosion( const Vec2& initialPosition, float orientationDegrees, float explosionSize, EntityType entityType, EntityFaction entityFaction, const SpriteAnimDefinition& spriteAnimDef, Game* game);
	~Explosion() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;


private:
	float m_timeSinceStartOfExplosion = 0.f;
	float m_durationOfExplosionSeconds = 2.f;
	float m_explosionSize = 1.f;
	const SpriteAnimDefinition& m_spriteAnimDef;


};
