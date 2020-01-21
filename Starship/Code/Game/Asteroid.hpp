#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class Asteroid : public Entity
{
public:
	Asteroid() {}
	Asteroid(Game* game);
	Asteroid( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game );
	~Asteroid();


	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void ActivateAsteroid();
private:
	Vec3 m_CompleteAsteroid[16];
};
