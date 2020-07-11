#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>

class AudioDefinition;
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
	AudioDefinition* GetAudioDefinition() const;
	Vec2 const& GetWeaponOffset( float orientationDegrees, bool m_isMoving ) const;

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

	Vec2 m_weaponOffsetIdleEast;
	Vec2 m_weaponOffsetIdleWest;
	Vec2 m_weaponOffsetIdleNorth;
	Vec2 m_weaponOffsetIdleSouth;
	Vec2 m_weaponOffsetIdleNorthEast;
	Vec2 m_weaponOffsetIdleNorthWest;
	Vec2 m_weaponOffsetIdleSouthEast;
	Vec2 m_weaponOffsetIdleSouthWest;
	Vec2 m_weaponOffsetMovingEast;
	Vec2 m_weaponOffsetMovingWest;
	Vec2 m_weaponOffsetMovingNorth;
	Vec2 m_weaponOffsetMovingSouth;
	Vec2 m_weaponOffsetMovingNorthEast;
	Vec2 m_weaponOffsetMovingNorthWest;
	Vec2 m_weaponOffsetMovingSouthEast;
	Vec2 m_weaponOffsetMovingSouthWest;

	float m_screenShakeIncrement = 0.f;
	
	BulletDefinition* m_bulletDef = nullptr;
	AudioDefinition* m_shootSound = nullptr;
};