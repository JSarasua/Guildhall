#include "Game/MapRegionType.hpp"
#include "Game/MapMaterialType.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::map< std::string, MapRegionType*> MapRegionType::s_definitions;
std::string MapRegionType::s_defaultMapRegion;

MapRegionType::MapRegionType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	m_isSolid = ParseXMLAttribute( element, "isSolid", false );

	m_isValid = g_theConsole->GuaranteeOrError( m_name != "INVALID", "Could not parse map region name" );
	if( !m_isValid )
	{
		return;
	}

	if( m_isSolid )
	{
		XmlElement const* sideElement = element.FirstChildElement( "Side" );
		m_isValid = g_theConsole->GuaranteeOrError( sideElement, "No Side element for map region" );
		if( !m_isValid )
		{
			return;
		}
		
		std::string sideMaterialName = ParseXMLAttribute( *sideElement, "material", "INVALID" );

		m_isValid = g_theConsole->GuaranteeOrError( sideMaterialName != "INVALID", "Material not found for Side" );
		if( !m_isValid )
		{
			return;
		}

		m_sideMaterialType = MapMaterialType::GetMapMaterialMatchingName( sideMaterialName );
	}
	else
	{
		XmlElement const* floorElement = element.FirstChildElement( "Floor" );
		XmlElement const* ceilingElement = element.FirstChildElement( "Ceiling" );

		m_isValid = g_theConsole->GuaranteeOrError( floorElement, "No Floor element for map region" );
		if( !m_isValid )
		{
			return;
		}

		m_isValid = g_theConsole->GuaranteeOrError( ceilingElement, "No Ceiling element for map region" );
		if( !m_isValid )
		{
			return;
		}

		std::string floorMaterialName = ParseXMLAttribute( *floorElement, "material", "INVALID" );
		std::string ceilingMaterialName = ParseXMLAttribute( *ceilingElement, "material", "INVALID" );

		m_isValid = g_theConsole->GuaranteeOrError( floorMaterialName != "INVALID", "Material not found for Floor" );
		if( !m_isValid )
		{
			return;
		}

		m_isValid = g_theConsole->GuaranteeOrError( ceilingMaterialName != "INVALID", "Material not found for Ceiling" );
		if( !m_isValid )
		{
			return;
		}

		m_floorMaterialType = MapMaterialType::GetMapMaterialMatchingName( floorMaterialName );
		m_ceilingMaterialType = MapMaterialType::GetMapMaterialMatchingName( ceilingMaterialName );
	}

	m_isValid = true;
}

bool MapRegionType::IsSolid() const
{
	return m_isSolid;
}

void MapRegionType::GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs, eMapMaterialArea mapMaterialArea ) const
{
	switch( mapMaterialArea )
	{
	case eMapMaterialArea::FLOOR:
		m_floorMaterialType->GetUVs( uvAtMins, uvAtMaxs );
		break;
	case eMapMaterialArea::CEILING:
		m_ceilingMaterialType->GetUVs( uvAtMins, uvAtMaxs );
		break;
	case eMapMaterialArea::SIDE:
		m_sideMaterialType->GetUVs( uvAtMins, uvAtMaxs );
		break;
	default:
		ERROR_AND_DIE("Invalid Map Material Area, should be floor, ceiling, or side" );
		break;
	}
}

Texture const& MapRegionType::GetTexture( eMapMaterialArea mapMaterialArea )
{
	switch( mapMaterialArea )
	{
	case eMapMaterialArea::FLOOR:
		return m_floorMaterialType->GetTexture();
		break;
	case eMapMaterialArea::CEILING:
		return m_ceilingMaterialType->GetTexture();
		break;
	case eMapMaterialArea::SIDE:
		return m_sideMaterialType->GetTexture();
		break;
	default:
		ERROR_AND_DIE( "Invalid Map Material Area, should be floor, ceiling, or side" );
		break;
	}
}

void MapRegionType::InitializeMapRegionDefinitions( const XmlElement& rootMapRegionElement )
{
	g_theConsole->PrintString(Rgba8::WHITE, "Loading Map Region Types..." );

	s_defaultMapRegion = ParseXMLAttribute( rootMapRegionElement, "default", "INVALID" );
	GUARANTEE_OR_DIE( s_defaultMapRegion != "INVALID", "Invalid default Map Region, cannot be INVALID" );
	
	for( const XmlElement* element = rootMapRegionElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string mapRegionName = nameAttribute->Value();
			MapRegionType* mapRegion = new MapRegionType( *element );
			if( mapRegion->m_isValid )
			{
				s_definitions[mapRegionName] = mapRegion;
			}
			else
			{
				delete mapRegion;
				mapRegion = nullptr;
			}
		}
	}
}

MapRegionType* MapRegionType::GetMapRegionTypeByString( std::string const& mapRegionName )
{
	auto mapRegionIter = s_definitions.find( mapRegionName );
	g_theConsole->GuaranteeOrError( mapRegionIter != s_definitions.end(), Stringf("Couldn't find map region: %s", mapRegionName.c_str() ) ); 

	if( mapRegionIter == s_definitions.end() )
	{
		mapRegionIter = s_definitions.find( MapRegionType::s_defaultMapRegion );
		GUARANTEE_OR_DIE( mapRegionIter != s_definitions.end(), "Couldn't find default map region by name" );
	}

	MapRegionType* mapRegionType = mapRegionIter->second;
	return mapRegionType;
}

