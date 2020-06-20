#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>

class BulletDefinition
{
public:
	BulletDefinition() = delete;
	BulletDefinition( XmlElement const& element );
	~BulletDefinition() {}

	static void InitializeBulletDefinitions( XmlElement const& rootBulletDefinitionElement );
	static std::map< std::string, BulletDefinition*> s_definitions;

protected:
	SpriteDefinition* m_bulletSpriteDef = nullptr;
	std::string m_name;
	AABB2 m_drawBounds;
	Rgba8 m_tint;
	float m_physicsRadius = 1.f;
	float m_bulletSpeed = 1.f;
	//int m_bulletBounce = 1.f;
	//explosionDef
	//explosion radius
};