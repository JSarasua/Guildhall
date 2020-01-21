#include "Game/Explosion.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/AABB2.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Explosion::Explosion( const Vec2& initialPosition, float orientationDegrees, float explosionSize, EntityType entityType, EntityFaction entityFaction, const SpriteAnimDefinition& spriteAnimDef, Game* game ) :
	m_explosionSize(explosionSize),
	m_spriteAnimDef(spriteAnimDef),
	Entity(entityType, entityFaction, game)
{
	m_position = initialPosition;
	m_orientationDegrees = orientationDegrees;
	m_timeSinceStartOfExplosion = 0.f;
	m_durationOfExplosionSeconds = EXPLOSION_DURATION;
}

void Explosion::Startup()
{

}

void Explosion::Update( float deltaSeconds )
{


	m_timeSinceStartOfExplosion += deltaSeconds;

	if( m_timeSinceStartOfExplosion > m_durationOfExplosionSeconds )
	{
		m_isGarbage = true;
	}
}

void Explosion::Render() const
{
	const SpriteDefinition& currentSpriteDef = m_spriteAnimDef.GetSpriteDefAtTime(m_timeSinceStartOfExplosion);
	Vec2 uvMins(0.f,0.f);
	Vec2 uvMaxs(1.f,1.f);
	currentSpriteDef.GetUVs(uvMins, uvMaxs);

	AABB2 aabb(Vec2(-m_explosionSize,-m_explosionSize), Vec2(m_explosionSize,m_explosionSize));

	aabb.Translate(m_position);

	
	Texture* explosionTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
	g_theRenderer->BindTexture( explosionTexture );
	//g_theRenderer->DrawAABB2Filled(aabb, Rgba8(255,255,255,255), uvMins, uvMaxs);
	g_theRenderer->DrawRotatedAABB2Filled(aabb, Rgba8(255,255,255,255), uvMins, uvMaxs, m_orientationDegrees);


	

}

