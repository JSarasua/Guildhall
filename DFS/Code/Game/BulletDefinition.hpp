#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>
#include "Engine/Renderer/RenderContext.hpp"

class BulletDefinition
{
public:
	BulletDefinition() = delete;
	BulletDefinition( XmlElement const& element );
	~BulletDefinition() {}

	static void InitializeBulletDefinitions( XmlElement const& rootBulletDefinitionElement );
	static std::map< std::string, BulletDefinition*> s_definitions;

	float GetBulletSpeed() const;
	float GetPhysicsRadius() const;
	AABB2 const& GetDrawBounds() const;
	Rgba8 const& GetTint() const;
	SpriteDefinition const* GetSpriteDefinition() const;
	eBlendMode const& GetBlendMode() const;
	int	GetBulletDamage() const;
	float GetLifeTime() const;

protected:
	SpriteDefinition* m_bulletSpriteDef = nullptr;
	std::string m_name;
	AABB2 m_drawBounds;
	Rgba8 m_tint;
	float m_physicsRadius = 1.f;
	float m_bulletSpeed = 1.f;
	eBlendMode m_blendMode = eBlendMode::ALPHA;
	int m_bulletDamage = 0;
	float m_lifeTime = 0.f;
	//int m_bulletBounce = 1.f;
	//explosionDef
	//explosion radius
};