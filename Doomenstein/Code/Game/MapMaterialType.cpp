#include "Game/MapMaterialType.hpp"


std::map< std::string, MapMaterialType*> MapMaterialType::s_definitions;

MapMaterialType::MapMaterialType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	m_floorSpriteUVs		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
	m_ceilingSpriteUVs		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
	m_sideSpriteUVs			= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
}

void MapMaterialType::InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement )
{
	for( const XmlElement* element = rootMapMaterialElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string mapMaterialName = nameAttribute->Value();
			MapMaterialType* mapMaterialType = new MapMaterialType( *element );
			s_definitions[mapMaterialName] = mapMaterialType;
		}
	}
}

