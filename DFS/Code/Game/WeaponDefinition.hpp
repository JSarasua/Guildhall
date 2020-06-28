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
	static WeaponDefinition const* GetRandomWeapon( RandomNumberGenerator& rng );
	static std::map< std::string, WeaponDefinition*> s_definitions;

	AABB2 const& GetWeaponDrawBounds() const;
	Rgba8 const& GetWeaponTint() const;
	SpriteDefinition const* GetWeaponSpriteDef() const;
	float GetShotsPerSecond() const;
	int GetBulletsPerShot() const;
	float GetBulletSpreadDegrees() const;
	BulletDefinition const* GetBulletDefinition() const;
	Vec2 const& GetTriggerPositionUV() const;
	Vec2 GetMuzzlePosition( float weaponOrientation ) const;
	Vec2 const& GetWeaponOffsetLeft() const;
	Vec2 const& GetWeaponOffsetRight() const;
	float GetScreenShakeIncremenet() const;

protected:
	SpriteDefinition* m_weaponSpriteDef = nullptr;
	std::string m_name;
	AABB2 m_drawBounds;
	Rgba8 m_tint;
	float m_shotsPerSecond = 0.f;
	int m_bulletsPerShot = 0;
	float m_bulletSpreadDegrees = 0.f;
	Vec2 m_triggerPosition;
	Vec2 m_muzzlePosition;
	Vec2 m_weaponOffsetRight;
	Vec2 m_weaponOffsetLeft;
	float m_screenShakeIncrement = 0.f;
	
	BulletDefinition* m_bulletDef = nullptr;
};