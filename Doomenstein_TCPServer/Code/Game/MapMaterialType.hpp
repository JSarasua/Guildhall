#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>

class Texture;
class SpriteSheet;
class SpriteDefinition;

class MapMaterialType
{
public:
	MapMaterialType() = delete;
	MapMaterialType( XmlElement const& element );
	~MapMaterialType() {}

	void GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs );
	Texture const& GetTexture();


	static void InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement );
	static MapMaterialType* GetMapMaterialMatchingName( std::string const& mapMaterialName );
	static std::map< std::string, SpriteSheet*> s_textures;
	static std::map< std::string, MapMaterialType*> s_definitions;
	static std::string s_defaultMapMaterial;

protected:
	std::string m_name;
	bool m_isValid = false;

	SpriteDefinition* m_spriteDefinition = nullptr;
	

};