#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::map< std::string, TileDefinition*> TileDefinition::s_definitions;

TileDefinition::TileDefinition( const XMLElement& element )
{

	m_name						= ParseXMLAttribute(element, "name", "INVALID");
	IntVec2 spriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2(0,0));
	m_tint						= ParseXMLAttribute(element, "spriteTint", Rgba8(255, 255, 255));
	m_allowsSight				= ParseXMLAttribute(element,"allowsSight", false);
	m_allowsWalking				= ParseXMLAttribute(element,"allowsWalking", false);
	m_allowsFlying				= ParseXMLAttribute(element,"allowsFlying", false);
	m_allowsSwimming			= ParseXMLAttribute(element,"allowsSwimming", false);


	int spriteIndex = g_tileSpriteSheet->GetSpriteIndex(spriteCoords);
	g_tileSpriteSheet->GetSpriteUVs(m_spriteUVs.mins, m_spriteUVs.maxs, spriteIndex);
}

void TileDefinition::InitializeTileDefinitions( const XMLElement& rootTileDefelement )
{
	for( const XMLElement* element = rootTileDefelement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XMLAttribute* nameAttribute = element->FindAttribute("name");
		
		if( nameAttribute )
		{
			std::string tileDefName = nameAttribute->Value();
			TileDefinition* tileDef = new TileDefinition(*element);
			s_definitions[tileDefName] = tileDef;
		}
	}
}

