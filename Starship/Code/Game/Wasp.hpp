#pragma once
#include "Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class Wasp : public Entity
{
public:
	Wasp() {}
	Wasp( Game* game );
	Wasp( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game );
	~Wasp() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	void ActivateWasp( Vec2 initialPosition, float initialOrientationDegrees );
};