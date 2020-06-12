#include "Game/MapMaterialType.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

extern RenderContext* g_theRenderer;

std::map< std::string, MapMaterialType*> MapMaterialType::s_definitions;
std::map< std::string, SpriteSheet*> MapMaterialType::s_textures;

MapMaterialType::MapMaterialType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	std::string spriteSheetName			= ParseXMLAttribute( element, "sheet", "INVALID" );
	IntVec2 floorSpriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
	
	GUARANTEE_OR_DIE( spriteSheetName != "INVALID", "Could not parse Spritesheet for material" );
	GUARANTEE_OR_DIE( floorSpriteCoords != IntVec2(-1, -1), "Invalid floor sprite coords for material" );
	
	SpriteSheet* spriteSheet = s_textures[spriteSheetName];
	int spriteIndex = spriteSheet->GetSpriteIndex( floorSpriteCoords );
	Vec2 uvAtMins, uvAtMaxs;
	spriteSheet->GetSpriteUVs( uvAtMins, uvAtMaxs, spriteIndex );

	m_spriteDefinition = new SpriteDefinition( *spriteSheet, spriteIndex, uvAtMins, uvAtMaxs );
	
//	IntVec2 ceilingSpriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
// 	IntVec2 sideSpriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
}

void MapMaterialType::InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement )
{
	const XmlElement* materialSheetElement = rootMapMaterialElement.FirstChildElement();
	std::string materialSheetName = materialSheetElement->Name();
	GUARANTEE_OR_DIE( materialSheetName == "MaterialsSheet", "MaterialsSheet must be first element in MapMaterialTypes" );

	const XmlAttribute* materialSheetNameAttribute = materialSheetElement->FindAttribute( "name" );
	if( materialSheetNameAttribute )
	{
		std::string sheetName = materialSheetNameAttribute->Value();

		XmlElement const* imageElement = materialSheetElement->FirstChildElement( "Diffuse" );
		std::string imageFilePath = ParseXMLAttribute( *imageElement, "image", "INVALID" );
		IntVec2 imageDimensions = ParseXMLAttribute( *materialSheetElement, "layout", IntVec2() );
		GUARANTEE_OR_DIE( imageFilePath != "INVALID", "Invalid Image File path for diffuse sheet" );
		GUARANTEE_OR_DIE( imageDimensions != IntVec2(), "Invalid Image dimensions for diffuse sheet" );

		Texture* texture = g_theRenderer->CreateOrGetTextureFromFile( imageFilePath.c_str() );
		SpriteSheet* spriteSheet = new SpriteSheet( *texture, imageDimensions );

		s_textures[sheetName] = spriteSheet;
			
	}


	for( const XmlElement* materialTypeElement = rootMapMaterialElement.FirstChildElement( "MaterialType" ); materialTypeElement; materialTypeElement=materialTypeElement->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = materialTypeElement->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string mapMaterialName = nameAttribute->Value();
			MapMaterialType* mapMaterialType = new MapMaterialType( *materialTypeElement );
			s_definitions[mapMaterialName] = mapMaterialType;
		}
	}
}

