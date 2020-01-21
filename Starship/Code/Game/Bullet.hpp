#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class Bullet : public Entity
{
public:
	Bullet() {}
	Bullet(Game* game);
	Bullet(Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game);
	~Bullet();


	virtual void Startup() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void ActivateBullet( Vec2 initialPosition, float initialOrientationDegrees, bool isABouncingBullet );

	void Bounce();
	void setColor( Rgba8 color );
private:
	bool m_isABouncingBullet = false;
};
