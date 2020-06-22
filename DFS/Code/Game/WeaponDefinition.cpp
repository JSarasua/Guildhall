#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BulletDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

std::map< std::string, WeaponDefinition*> WeaponDefinition::s_definitions;


AABB2 const& WeaponDefinition::GetWeaponDrawBounds() const
{
	return m_drawBounds;
}

Rgba8 const& WeaponDefinition::GetWeaponTint() const
{
	return m_tint;
}

SpriteDefinition const* WeaponDefinition::GetWeaponSpriteDef() const
{
	return m_weaponSpriteDef;
}

float WeaponDefinition::GetShotsPerSecond() const
{
	return m_shotsPerSecond;
}

int WeaponDefinition::GetBulletsPerShot() const
{
	return m_bulletsPerShot;
}

float WeaponDefinition::GetBulletSpreadDegrees() const
{
	return m_bulletSpreadDegrees;
}

BulletDefinition const* WeaponDefinition::GetBulletDefinition() const
{
	return m_bulletDef;
}

Vec2 const& WeaponDefinition::GetTriggerPositionUV() const
{
	return m_triggerPosition;
}

Vec2 WeaponDefinition::GetMuzzlePosition( float weaponOrientation ) const
{
	Vec2 localTriggerPosition = m_drawBounds.GetPointAtUV( m_triggerPosition );
	//Vec2 localTriggerPosition = m_drawBounds.GetPointAtUV( Vec2( 0.5f, 0.5f ) );
	Vec2 muzzleUV = m_muzzlePosition;
	if( GetShortestAngularDistance( 180.f, weaponOrientation ) <  90.f )
	{
		muzzleUV.y = 1.f - muzzleUV.y;
	}
	Vec2 localMuzzlePosition = m_drawBounds.GetPointAtUV( muzzleUV );
	Vec2 triggerToMuzzle = localMuzzlePosition - localTriggerPosition;
	Vec2 muzzlePosition = triggerToMuzzle.GetRotatedDegrees( weaponOrientation );

	return muzzlePosition;
}

Vec2 const& WeaponDefinition::GetWeaponOffsetLeft() const
{
	return m_weaponOffsetLeft;
}

Vec2 const& WeaponDefinition::GetWeaponOffsetRight() const
{
	return m_weaponOffsetRight;
}

WeaponDefinition::WeaponDefinition( XmlElement const& element )
{
	m_name						= ParseXMLAttribute( element, "name", "INVALID" );
	IntVec2 spriteCoords		= ParseXMLAttribute( element, "spriteCoords", IntVec2( 0, 0 ) );
	m_tint						= ParseXMLAttribute( element, "spriteTint", Rgba8( 255, 255, 255 ) );
	m_drawBounds.mins			= ParseXMLAttribute( element, "minDrawBounds", Vec2( 0.f, 0.f ) );
	m_drawBounds.maxs			= ParseXMLAttribute( element, "maxDrawBounds", Vec2( 0.f, 0.f ) );
	m_shotsPerSecond			= ParseXMLAttribute( element, "shotsPerSecond", 1.f );
	m_bulletsPerShot			= ParseXMLAttribute( element, "bulletsPerShot", 1 );
	m_bulletSpreadDegrees		= ParseXMLAttribute( element, "bulletSpreadDegrees", 10.f );
	m_triggerPosition			= ParseXMLAttribute( element, "triggerPosition", Vec2( 0.5f, 0.5f ) );
	m_muzzlePosition			= ParseXMLAttribute( element, "muzzlePosition", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetRight			= ParseXMLAttribute( element, "weaponOffsetRight", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetLeft			= ParseXMLAttribute( element, "weaponOffsetLeft", Vec2( 0.5f, 0.5f ) );
	std::string bulletType		= ParseXMLAttribute( element, "bulletType", "INVALID" );


	int spriteIndex = g_weaponSpriteSheet->GetSpriteIndex( spriteCoords );
	AABB2 spriteUVs;
	g_weaponSpriteSheet->GetSpriteUVs( spriteUVs.mins, spriteUVs.maxs, spriteIndex );

	m_weaponSpriteDef = new SpriteDefinition( *g_weaponSpriteSheet, spriteIndex, spriteUVs.mins, spriteUVs.maxs );

	m_bulletDef = BulletDefinition::s_definitions[bulletType];
}

void WeaponDefinition::InitializeWeaponDefinitions( XmlElement const& rootWeaponDefinitionElement )
{
	for( const XmlElement* element = rootWeaponDefinitionElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string weaponDefName = nameAttribute->Value();
			WeaponDefinition* weaponDef = new WeaponDefinition( *element );
			s_definitions[weaponDefName] = weaponDef;
		}
	}
}

