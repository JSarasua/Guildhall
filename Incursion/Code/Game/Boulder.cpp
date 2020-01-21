#include "Game/Boulder.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Boulder::Boulder( const Vec2& initialPosition, Game* game ) :
	Entity(game)
{
	m_position = initialPosition;

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = BOULDER_HEALTH;
}

Boulder::Boulder( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game ) :
		Entity(type, faction, game)
{
	m_position = initialPosition;
	m_physicsRadius = BOULDER_PHYSICS_RADIUS;
	m_cosmeticRadius = BOULDER_COSMETIC_RADIUS;
	m_orientationDegrees = orientationDegrees;

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = BOULDER_HEALTH;
}

void Boulder::Startup()
{

}

void Boulder::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Boulder::Render() const
{
	Texture* m_boulderTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Extras_4x4.png" );
	SpriteSheet* extrasSpriteSheet = new SpriteSheet( *m_boulderTexture, IntVec2( 4, 4 ) );

	const SpriteDefinition& boulderSpriteDef = extrasSpriteSheet->GetSpriteDefinition( 3 );
	
	Vec2 uvMins;
	Vec2 uvMaxs;

	boulderSpriteDef.GetUVs(uvMins, uvMaxs);

	AABB2 boulderAABB2 = AABB2( Vec2( -m_cosmeticRadius, -m_cosmeticRadius ), Vec2( m_cosmeticRadius, m_cosmeticRadius ) );
	Vertex_PCU boulderVertexes[] =
	{
		Vertex_PCU( Vec3( boulderAABB2.mins ),Rgba8( 255,255,255,255 ), uvMins ),
		Vertex_PCU( Vec3( Vec2( boulderAABB2.maxs.x, boulderAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( uvMaxs.x, uvMins.y ) ),
		Vertex_PCU( Vec3( boulderAABB2.maxs ),Rgba8( 255,255,255,255 ), uvMaxs ),

		Vertex_PCU( Vec3( boulderAABB2.mins ),Rgba8( 255,255,255,255 ), uvMins ),
		Vertex_PCU( Vec3( boulderAABB2.maxs ),Rgba8( 255,255,255,255 ), uvMaxs ),
		Vertex_PCU( Vec3( Vec2( boulderAABB2.mins.x, boulderAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( uvMins.x, uvMaxs.y ) )

	};


	//Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	Vertex_PCU::TransformVertexArray( 6, boulderVertexes, 1.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture( m_boulderTexture );
	g_theRenderer->DrawVertexArray( 6, boulderVertexes );



	Entity::DebugRender();
}

