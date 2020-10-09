#pragma once
#include "Game/Entity.hpp"
#include "Engine/Time/Timer.hpp"

class Game;
class BulletDefinition;


class Bullet : public Entity
{
public:

	Bullet( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, BulletDefinition const* bulletDef, float speedMultiplier, Vec2 const& entityVelocity );
	~Bullet() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	int GetBulletDamage() const;


private:
	BulletDefinition const* m_bulletDefinition = nullptr;
	Timer m_lifetime;
};