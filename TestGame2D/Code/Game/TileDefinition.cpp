#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::map< std::string, TileDefinition*> TileDefinition::s_definitions;

TileDefinition::TileDefinition( const XmlElement& element )
{

	m_name						= ParseXMLAttribute(element, "name", "INVALID");
	IntVec2 spriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2(0,0));
	m_tint						= ParseXMLAttribute(element, "spriteTint", Rgba8(255, 255, 255));
	m_tileGlyphColor			= ParseXMLAttribute(element, "tileGlyphColor", Rgba8(0,0,0,0));
	m_allowsSight				= ParseXMLAttribute(element,"allowsSight", false);
	m_allowsWalking				= ParseXMLAttribute(element,"allowsWalking", false);
	m_allowsFlying				= ParseXMLAttribute(element,"allowsFlying", false);
	m_allowsSwimming			= ParseXMLAttribute(element,"allowsSwimming", false);

	int spriteIndex = g_tileSpriteSheet->GetSpriteIndex(spriteCoords);
	g_tileSpriteSheet->GetSpriteUVs(m_spriteUVs.mins, m_spriteUVs.maxs, spriteIndex);
}

void TileDefinition::InitializeTileDefinitions( const XmlElement& rootTileDefelement )
{
	for( const XmlElement* element = rootTileDefelement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute("name");
		
		if( nameAttribute )
		{
			std::string tileDefName = nameAttribute->Value();
			TileDefinition* tileDef = new TileDefinition(*element);
			s_definitions[tileDefName] = tileDef;
		}
	}
}

TileDefinition* TileDefinition::GetTileDefFromColor( const Rgba8& colorValue )
{
	for( TileDefIterator iter = s_definitions.begin(); iter != s_definitions.end(); iter++ )
	{
		TileDefinition* tileDef = iter->second;
		if( tileDef->m_tileGlyphColor == colorValue )
		{
			return tileDef;
		}
	}
	return nullptr;
}

