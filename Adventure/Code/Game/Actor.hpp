#pragma once
#include <string>
#include "Game/Entity.hpp"

class ActorDefinition;

class Actor : public Entity
{
	friend class Map;
public:
	Actor() = delete;
	Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition* actorDef);
	~Actor(){}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

protected:
	void UpdateFromJoystick( float deltaSeconds );
	void UpdateFromKeyboard( float deltaSeconds );

protected:
	std::string m_name;
	ActorDefinition* m_actorDef = nullptr;
};