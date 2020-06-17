#include "Game/MapMaterialType.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"

extern RenderContext* g_theRenderer;

std::map< std::string, MapMaterialType*> MapMaterialType::s_definitions;
std::map< std::string, SpriteSheet*> MapMaterialType::s_textures;
std::string MapMaterialType::s_defaultMapMaterial;

MapMaterialType::MapMaterialType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	std::string spriteSheetName			= ParseXMLAttribute( element, "sheet", "INVALID" );
	IntVec2 materialSpriteCoords		= ParseXMLAttribute(element, "spriteCoords", IntVec2( -1,-1 ) );
	
	m_isValid = g_theConsole->GuaranteeOrError( spriteSheetName != "INVALID", Stringf("Could not parse Spritesheet name for material: %s", m_name.c_str() ) );
	if( !m_isValid )
	{
		return;
	}
	m_isValid = g_theConsole->GuaranteeOrError( materialSpriteCoords != IntVec2( -1, -1 ), Stringf("Could not parse sprite coordinates for material: %s", m_name.c_str() ) );
	if( !m_isValid )
	{
		return;
	}
	
	SpriteSheet* spriteSheet = s_textures[spriteSheetName];
	int spriteIndex = spriteSheet->GetSpriteIndex( materialSpriteCoords );
	Vec2 uvAtMins, uvAtMaxs;
	spriteSheet->GetSpriteUVs( uvAtMins, uvAtMaxs, spriteIndex );

	m_spriteDefinition = new SpriteDefinition( *spriteSheet, spriteIndex, uvAtMins, uvAtMaxs );
}

void MapMaterialType::GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs )
{
	m_spriteDefinition->GetUVs( uvAtMins, uvAtMaxs );
}

Texture const& MapMaterialType::GetTexture()
{
	return m_spriteDefinition->GetTexture();
}


void MapMaterialType::InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement )
{
	g_theConsole->PrintString(Rgba8::WHITE, "Loading Map Material Types..." );
	std::string mapMaterialRootName = rootMapMaterialElement.Name();
	g_theConsole->GuaranteeOrError( mapMaterialRootName == "MapMaterialTypes", Stringf( "ERROR: Expected MapMaterialTypes as root node" ) );

	s_defaultMapMaterial = ParseXMLAttribute( rootMapMaterialElement, "default", "INVALID" );
	g_theConsole->GuaranteeOrError( s_defaultMapMaterial != "INVALID", "ERROR: Invalid default Map material, cannot be INVALID" );

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

			if( mapMaterialType->m_isValid )
			{
				s_definitions[mapMaterialName] = mapMaterialType;
			}
			else
			{
				delete mapMaterialType;
				mapMaterialType = nullptr;
			}

		}
	}
}

MapMaterialType* MapMaterialType::GetMapMaterialMatchingName( std::string const& mapMaterialName )
{
	auto mapMaterialIter = s_definitions.find( mapMaterialName );
	g_theConsole->GuaranteeOrError( mapMaterialIter != s_definitions.end(), Stringf("ERROR: Couldn't find map material: %s", mapMaterialName.c_str() ) );

	if( mapMaterialIter == s_definitions.end() )
	{
		mapMaterialIter = s_definitions.find( MapMaterialType::s_defaultMapMaterial );

		if( mapMaterialIter == s_definitions.end() )
		{
			return nullptr;
		}
		//GUARANTEE_OR_DIE( mapMaterialIter != s_definitions.end(), "ERROR: Couldn't find default map material by name" );
	}

	return mapMaterialIter->second;
}

