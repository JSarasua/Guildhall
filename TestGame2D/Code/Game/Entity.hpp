#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
class Game;

class Entity
{
public:
	friend class Map;
	~Entity() {}
	Entity(Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity);

	virtual void Startup() = 0;
	virtual void Update(float deltaSeconds);
	virtual void Render() const = 0;

	bool IsOffScreen();
	Vec2 GetForwardVector();
	bool IsAlive();
	void SetAlive();
	const Vec2 GetPosition();
	virtual void Lose1Health();
	virtual void LoseAllHealth();
	const Rgba8& GetColor();

	void SetPosition(const Vec2& newPosition);



protected:
	Vec2 m_position = Vec2(0.f,0.f);
	Vec2 m_velocity = Vec2(0.f,0.f);
	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 1000.f;
	float m_physicsRadius = 1000.f;
	float m_cosmeticRadius = 2000.f;
	int m_health = 0;
	bool m_isDead = true;
	Rgba8 m_color = Rgba8(255,255,255);
	bool m_hasCollided = false;

	AABB2 m_drawBounds;
	bool m_canWalk = false;
	bool m_canFly = false;
	bool m_canSwim = false;
};