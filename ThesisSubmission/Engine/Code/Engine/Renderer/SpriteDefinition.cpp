#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"



SpriteDefinition::SpriteDefinition( const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs ) :
	m_spriteSheet(spriteSheet),
	m_spriteIndex(spriteIndex),
	m_uvAtMins(uvAtMins),
	m_uvAtMaxs(uvAtMaxs)
{

}

void SpriteDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins = m_uvAtMins;
	out_uvAtMaxs = m_uvAtMaxs;
}

const SpriteSheet& SpriteDefinition::GetSpriteSheet() const
{
	return m_spriteSheet;
}

const Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

float SpriteDefinition::GetAspect() const
{
	float widthDisplacement = m_uvAtMaxs.x - m_uvAtMins.x;
	float heightDisplacement = m_uvAtMaxs.y - m_uvAtMins.y;
	float aspectRatio = widthDisplacement/heightDisplacement;

	Vec2 texelSizeCoords(GetTexture().GetTexelSize());
	float textureAspectRatio = texelSizeCoords.x/texelSizeCoords.y;
	aspectRatio *= textureAspectRatio;
	return aspectRatio;
}

