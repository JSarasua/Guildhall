#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"

class Game;
class EntityDefinition;

class Entity
{
public:
	Entity() {}
	virtual ~Entity() {}
	Entity(  EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees );

	virtual void Startup() = 0;
	virtual void Update(float deltaSeconds);
	virtual void Render() const = 0;

	bool IsOffScreen();
	Vec3 GetForwardVector() const;
	bool IsAlive();
	void SetAlive();
	const Vec2 GetPosition();
	Vec3 GetEyeHeightPosition();
	virtual void Lose1Health();
	virtual void LoseAllHealth();
	const Rgba8& GetColor();

	void SetPosition(const Vec2& newPosition);

	bool IsPossessed() const;
	void SetIsPossessed( bool isPossessed );

protected:
	Vec2 m_position = Vec2(0.f,0.f);
	//Vec2 m_velocity = Vec2(0.f,0.f);
	//float m_orientationDegrees = 0.f;
	//float m_angularVelocity = 1000.f;
	//float m_physicsRadius = 1000.f;
	float m_cosmeticRadius = 2000.f;
	int m_health = 0;
	bool m_isDead = true;
	Rgba8 m_color = Rgba8(255,255,255);


	EntityDefinition const* m_entityDef = nullptr;
	Vec3 m_pitchRollYawDegrees;

	bool m_isPossessed = false;
};