#include "Game/BulletDefinition.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, BulletDefinition*> BulletDefinition::s_definitions;


float BulletDefinition::GetBulletSpeed() const
{
	return m_bulletSpeed;
}

float BulletDefinition::GetPhysicsRadius() const
{
	return m_physicsRadius;
}

AABB2 const& BulletDefinition::GetDrawBounds() const
{
	return m_drawBounds;
}

Rgba8 const& BulletDefinition::GetTint() const
{
	return m_tint;
}

SpriteDefinition const* BulletDefinition::GetSpriteDefinition() const
{
	return m_bulletSpriteDef;
}

eBlendMode const& BulletDefinition::GetBlendMode() const
{
	return m_blendMode;
}

int BulletDefinition::GetBulletDamage() const
{
	return m_bulletDamage;
}

BulletDefinition::BulletDefinition( XmlElement const& element )
{
	m_name						= ParseXMLAttribute( element, "name", "INVALID" );
	IntVec2 spriteCoords		= ParseXMLAttribute( element, "spriteCoords", IntVec2( 0, 0 ) );
	m_tint						= ParseXMLAttribute( element, "spriteTint", Rgba8( 255, 255, 255 ) );
	m_drawBounds.mins			= ParseXMLAttribute( element, "minDrawBounds", Vec2( 0.f, 0.f ) );
	m_drawBounds.maxs			= ParseXMLAttribute( element, "maxDrawBounds", Vec2( 0.f, 0.f ) );
	m_physicsRadius				= ParseXMLAttribute( element, "physicsRadius", 1.f );
	m_bulletSpeed				= ParseXMLAttribute( element, "bulletSpeed", 1.f );
	m_bulletDamage				= ParseXMLAttribute( element, "damage", 0 );
	std::string blendMode		= ParseXMLAttribute( element, "blendMode", "ALPHA" );

	if( blendMode == "ALPHA" )
	{
		m_blendMode = eBlendMode::ALPHA;
	}
	else if( blendMode == "ADDITIVE" )
	{
		m_blendMode = eBlendMode::ADDITIVE;
	}

	int spriteIndex = g_bulletsSpriteSheet->GetSpriteIndex( spriteCoords );
	AABB2 spriteUVs;
	g_bulletsSpriteSheet->GetSpriteUVs( spriteUVs.mins, spriteUVs.maxs, spriteIndex );

	m_bulletSpriteDef = new SpriteDefinition( *g_bulletsSpriteSheet, spriteIndex, spriteUVs.mins, spriteUVs.maxs );
}

void BulletDefinition::InitializeBulletDefinitions( XmlElement const& rootBulletDefinitionElement )
{
	for( const XmlElement* element = rootBulletDefinitionElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string bulletDefName = nameAttribute->Value();
			BulletDefinition* bulletDef = new BulletDefinition( *element );
			s_definitions[bulletDefName] = bulletDef;
		}
	}
}

