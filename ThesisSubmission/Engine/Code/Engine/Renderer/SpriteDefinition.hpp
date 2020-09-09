#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Texture.hpp"


//UVs of a single sprite on a sprite sheet

class SpriteSheet;

class SpriteDefinition
{
public:
	explicit SpriteDefinition( const SpriteSheet&, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs );

	void				GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const;
	const SpriteSheet&	GetSpriteSheet() const;
	const Texture&		GetTexture() const;
	float				GetAspect() const;


protected:
	const SpriteSheet&	m_spriteSheet;
	int					m_spriteIndex	= -1;
	Vec2				m_uvAtMins		= Vec2(0.f,0.f);
	Vec2				m_uvAtMaxs		= Vec2(1.f,1.f);

};