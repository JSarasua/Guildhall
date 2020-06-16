#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"



SpriteSheet::SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout ) :
	m_texture(texture)
{
	m_spriteSheetDimensions = simpleGridLayout;

	const int widthOfTexture = simpleGridLayout.x;
	const int heightOfTexture = simpleGridLayout.y;
	const float spriteUs = 1.f/static_cast<float>(widthOfTexture);
	const float spriteVs = 1.f/static_cast<float>(heightOfTexture);

	//loop through all textures and cut
	int spriteDefIndex = 0;
	for( int heightIndex = heightOfTexture - 1; heightIndex >= 0; heightIndex-- )
	{
		for( int widthIndex = 0; widthIndex < widthOfTexture; widthIndex++ )
		{
			const float widthUnit = static_cast<float>(widthIndex);
			const float heightUnit = static_cast<float>(heightIndex);


			Vec2 uvMins(widthUnit*spriteUs, heightUnit*spriteVs);
			Vec2 uvMaxs((widthUnit+1.f) * spriteUs, (heightUnit+1.f) * spriteVs);
			m_spriteDefs.push_back(SpriteDefinition(*this, spriteDefIndex, uvMins, uvMaxs ));
			spriteDefIndex++;
		}
	}
}

const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefs[spriteIndex];
}

const int SpriteSheet::GetSpriteIndex( const IntVec2& spriteCoords ) const
{
	if( spriteCoords.x < m_spriteSheetDimensions.x && spriteCoords.y < m_spriteSheetDimensions.y &&
		spriteCoords.x >= 0 && spriteCoords.y >= 0)
	{
		const int spriteIndex = m_spriteSheetDimensions.x * spriteCoords.y + spriteCoords.x;
		return spriteIndex;
	}
	g_theConsole->ErrorString( Stringf("ERROR: Invalid sprite coordinates for sprite sheet: (%i,%i)", spriteCoords.x, spriteCoords.y ) );
	return 0;
}

void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	m_spriteDefs[spriteIndex].GetUVs(out_uvAtMins,out_uvAtMaxs);
}

