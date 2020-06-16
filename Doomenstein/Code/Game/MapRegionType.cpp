#include "Game/MapRegionType.hpp"
#include "Game/MapMaterialType.hpp"

std::map< std::string, MapRegionType*> MapRegionType::s_definitions;
std::string MapRegionType::s_defaultMapRegion;

MapRegionType::MapRegionType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	m_isSolid = ParseXMLAttribute( element, "isSolid", false );

	//GUARANTEE_OR_DIE( m_name != "INVALID", "Could not parse MapRegion name" );
	if( m_name == "INVALID" )
	{
		m_isValid = false;
		return;
	}

	if( m_isSolid )
	{
		XmlElement const* sideElement = element.FirstChildElement( "Side" );
		GUARANTEE_OR_DIE( sideElement, "No Side element for map region" );
		
		std::string sideMaterialName = ParseXMLAttribute( *sideElement, "material", "INVALID" );
		GUARANTEE_OR_DIE( sideMaterialName != "INVALID", "Material not found for Side" );

		m_sideMaterialType = MapMaterialType::s_definitions[sideMaterialName];
	}
	else
	{
		XmlElement const* floorElement = element.FirstChildElement( "Floor" );
		XmlElement const* ceilingElement = element.FirstChildElement( "Ceiling" );
		GUARANTEE_OR_DIE( floorElement, "No Floor element for map region" );
		GUARANTEE_OR_DIE( ceilingElement, "No Ceiling element for map region" );

		std::string floorMaterialName = ParseXMLAttribute( *floorElement, "material", "INVALID" );
		std::string ceilingMaterialName = ParseXMLAttribute( *ceilingElement, "material", "INVALID" );
		GUARANTEE_OR_DIE( floorMaterialName != "INVALID", "Material not found for Floor" );
		GUARANTEE_OR_DIE( ceilingMaterialName != "INVALID", "Material not found for Floor" );

		m_floorMaterialType = MapMaterialType::s_definitions[floorMaterialName];
		m_ceilingMaterialType = MapMaterialType::s_definitions[ceilingMaterialName];
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
				s_definitions[mapRegionName] = s_definitions[s_defaultMapRegion];
			}
		}
	}
}

MapRegionType* MapRegionType::GetMapRegionTypeByString( std::string const& mapRegionName )
{
	auto mapRegionIter = s_definitions.find( mapRegionName );
	//GUARANTEE_OR_DIE( mapRegionIter != s_definitions.end(), "Couldn't find map region by name" );

	if( mapRegionIter == s_definitions.end() )
	{
		mapRegionIter = s_definitions.find( MapRegionType::s_defaultMapRegion );
		GUARANTEE_OR_DIE( mapRegionIter != s_definitions.end(), "Couldn't find default map region by name" );
	}

	MapRegionType* mapRegionType = mapRegionIter->second;
	return mapRegionType;
}

