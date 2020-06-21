#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BulletDefinition.hpp"

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

