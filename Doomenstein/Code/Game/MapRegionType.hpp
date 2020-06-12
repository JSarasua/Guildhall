#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>

enum class eMapMaterialArea
{
	FLOOR,
	CEILING,
	SIDE
};

class MapMaterialType;
class Texture;

class MapRegionType
{
public:
	MapRegionType() = delete;
	MapRegionType( XmlElement const& element );
	~MapRegionType(){}

	bool IsSolid() const;
	void GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs, eMapMaterialArea mapMaterialArea ) const;
	Texture const& GetTexture( eMapMaterialArea mapMaterialArea );
	
	static void InitializeMapRegionDefinitions( const XmlElement& rootMapRegionElement );
	static MapRegionType* GetMapRegionTypeByString( std::string const& mapRegionName );
	static std::map< std::string, MapRegionType*> s_definitions;


protected:
	std::string m_name;
	bool m_isSolid = false;

	MapMaterialType* m_floorMaterialType = nullptr;
	MapMaterialType* m_ceilingMaterialType = nullptr;
	MapMaterialType* m_sideMaterialType = nullptr;
	//Floor
	//Ceiling
	//Side
};