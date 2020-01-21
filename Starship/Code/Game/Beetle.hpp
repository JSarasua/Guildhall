#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class Beetle : public Entity
{
public:
	Beetle() {}
	Beetle(Game* game);
	Beetle(Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game);
	~Beetle() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	void ActivateBeetle( Vec2 initialPosition, float initialOrientationDegrees );
};