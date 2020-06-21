#pragma once
#include "Game/Entity.hpp"

class Game;
class BulletDefinition;


class Bullet : public Entity
{
public:

	Bullet( const Vec2& initialPosition, BulletDefinition const* bulletDef );
	Bullet( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, BulletDefinition const* bulletDef );
	~Bullet() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	int GetBulletDamage() const;


private:
	BulletDefinition const* m_bulletDefinition = nullptr;

};