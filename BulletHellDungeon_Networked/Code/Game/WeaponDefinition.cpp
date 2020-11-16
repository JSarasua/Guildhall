#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BulletDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/AudioDefinition.hpp"

std::map< std::string, WeaponDefinition*> WeaponDefinition::s_definitions;


int WeaponDefinition::GetWeaponDefIndex( WeaponDefinition const* weaponDef )
{
	int index = 0;
	for( auto weaponDefIter : s_definitions )
	{
		if( weaponDefIter.second == weaponDef )
		{
			break;
		}
		index++;
	}

	return index;
}

WeaponDefinition const* WeaponDefinition::GetWeaponDefFromIndex( int weaponIndex )
{
	WeaponDefinition const* weaponDef = nullptr;
	int currentIndex = 0;
	for( auto weaponDefIter : s_definitions )
	{
		if( currentIndex == weaponIndex )
		{
			weaponDef = weaponDefIter.second;
			break;
		}
		currentIndex++;
	}

	return weaponDef;
}

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

float WeaponDefinition::GetScreenShakeIncremenet() const
{
	return m_screenShakeIncrement;
}

AudioDefinition* WeaponDefinition::GetAudioDefinition() const
{
	return m_shootSound;
}

Vec2 const& WeaponDefinition::GetWeaponOffset( float orientationDegrees, bool m_isMoving ) const
{
	if( m_isMoving )
	{
		if( GetShortestAngularDistance( 0.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetMovingEast;
		}
		else if( GetShortestAngularDistance( 45.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetMovingNorthEast;
		}
		else if( GetShortestAngularDistance( 90.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetMovingNorth;
		}
		else if( GetShortestAngularDistance( 135.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetMovingNorthWest;
		}
		else if( GetShortestAngularDistance( 180.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetMovingWest;
		}
		else if( GetShortestAngularDistance( 225.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetMovingSouthWest;
		}
		else if( GetShortestAngularDistance( 270.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetMovingSouth;
		}
		else if( GetShortestAngularDistance( 315.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetMovingSouthEast;
		}
	}
	else
	{
		if( GetShortestAngularDistance( 0.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetIdleEast;
		}
		else if( GetShortestAngularDistance( 45.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetIdleNorthEast;
		}
		else if( GetShortestAngularDistance( 90.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetIdleNorth;
		}
		else if( GetShortestAngularDistance( 135.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetIdleNorthWest;
		}
		else if( GetShortestAngularDistance( 180.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetIdleWest;
		}
		else if( GetShortestAngularDistance( 225.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetIdleSouthWest;
		}
		else if( GetShortestAngularDistance( 270.f, orientationDegrees ) <= 22.5f )
		{
			return m_weaponOffsetIdleSouth;
		}
		else if( GetShortestAngularDistance( 315.f, orientationDegrees ) < 22.5f )
		{
			return m_weaponOffsetIdleSouthEast;
		}
	}

	//Should never be here.
	return m_weaponOffsetIdleEast;
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
	
	m_weaponOffsetIdleEast				= ParseXMLAttribute( element, "weaponOffsetIdleEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleWest				= ParseXMLAttribute( element, "weaponOffsetIdleWest", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleNorth				= ParseXMLAttribute( element, "weaponOffsetIdleNorth", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleSouth				= ParseXMLAttribute( element, "weaponOffsetIdleSouth", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleNorthEast			= ParseXMLAttribute( element, "weaponOffsetIdleNorthEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleNorthWest			= ParseXMLAttribute( element, "weaponOffsetIdleNorthWest", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleSouthEast			= ParseXMLAttribute( element, "weaponOffsetIdleSouthEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetIdleSouthWest			= ParseXMLAttribute( element, "weaponOffsetIdleSouthWest", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingEast			= ParseXMLAttribute( element, "weaponOffsetMovingEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingWest			= ParseXMLAttribute( element, "weaponOffsetMovingWest", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingNorth			= ParseXMLAttribute( element, "weaponOffsetMovingNorth", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingSouth			= ParseXMLAttribute( element, "weaponOffsetMovingSouth", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingNorthEast		= ParseXMLAttribute( element, "weaponOffsetMovingNorthEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingNorthWest		= ParseXMLAttribute( element, "weaponOffsetMovingNorthWest", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingSouthEast		= ParseXMLAttribute( element, "weaponOffsetMovingSouthEast", Vec2( 0.5f, 0.5f ) );
	m_weaponOffsetMovingSouthWest		= ParseXMLAttribute( element, "weaponOffsetMovingSouthWest", Vec2( 0.5f, 0.5f ) );

	
	m_screenShakeIncrement		= ParseXMLAttribute( element, "screenshakeIncrement", 0.f );
	std::string bulletType		= ParseXMLAttribute( element, "bulletType", "INVALID" );
	std::string shootSoundStr	= ParseXMLAttribute( element, "shootSound", "INVALID" );

	int spriteIndex = g_weaponSpriteSheet->GetSpriteIndex( spriteCoords );
	AABB2 spriteUVs;
	g_weaponSpriteSheet->GetSpriteUVs( spriteUVs.mins, spriteUVs.maxs, spriteIndex );

	m_weaponSpriteDef = new SpriteDefinition( *g_weaponSpriteSheet, spriteIndex, spriteUVs.mins, spriteUVs.maxs );

	m_bulletDef = BulletDefinition::s_definitions[bulletType];
	m_shootSound = AudioDefinition::GetAudioDefinition( shootSoundStr );
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


WeaponDefinition const* WeaponDefinition::GetRandomWeapon( RandomNumberGenerator& rng )
{
	int numWeapons = (int)s_definitions.size() - 1;
	int weaponIndex = rng.RollRandomIntInRange( 0, numWeapons );
	auto iter = s_definitions.begin();
	std::advance( iter, weaponIndex );

	while( iter->second->m_name == "Pistol" )
	{
		weaponIndex = rng.RollRandomIntInRange( 0, numWeapons );
		iter = s_definitions.begin();
		std::advance( iter, weaponIndex );
	}
	return iter->second;
}

