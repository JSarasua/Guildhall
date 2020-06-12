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

	static void InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement );
	static std::map< std::string, SpriteSheet*> s_textures;
	static std::map< std::string, MapMaterialType*> s_definitions;

protected:
	std::string m_name;

	SpriteDefinition* m_spriteDefinition = nullptr;
	

};