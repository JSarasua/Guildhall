#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>

class BulletDefinition;

class WeaponDefinition
{
public:
	WeaponDefinition() = delete;
	WeaponDefinition( XmlElement const& element );
	~WeaponDefinition() {}

	static void InitializeWeaponDefinitions( XmlElement const& rootWeaponDefinitionElement );
	static std::map< std::string, WeaponDefinition*> s_definitions;

	AABB2 const& GetWeaponDrawBounds() const;
	Rgba8 const& GetWeaponTint() const;
	SpriteDefinition const* GetWeaponSpriteDef() const;
	float GetShotsPerSecond() const;
	int GetBulletsPerShot() const;
	float GetBulletSpreadDegrees() const;

protected:
	SpriteDefinition* m_weaponSpriteDef = nullptr;
	std::string m_name;
	AABB2 m_drawBounds;
	Rgba8 m_tint;
	float m_shotsPerSecond = 0.f;
	int m_bulletsPerShot = 0;
	float m_bulletSpreadDegrees = 0.f;
	
	BulletDefinition* m_bulletDef = nullptr;
};