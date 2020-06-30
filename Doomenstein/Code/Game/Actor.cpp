#include "Game/Actor.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

extern RenderContext* g_theRenderer;

Actor::Actor( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	Entity( entityDef, initialPosition, pitchRollYawDegrees )
{

}

void Actor::Update( float deltaSeconds )
{

}

void Actor::Render() const
{
	float eyeHeight = m_entityDef->GetEyeHeight();
	float physicsRadius = m_entityDef->GetPhysicsRadius();
	float height = m_entityDef->GetHeight();
	Vec3 position = Vec3( m_position, eyeHeight );
	DebugAddWorldWireCylinder( m_position, physicsRadius, height, Rgba8::WHITE, Rgba8::WHITE, 0.f );

	LineSegment3 forwardVec = LineSegment3( Vec3(m_position, eyeHeight), Vec3(m_position, eyeHeight) + GetForwardVector() );
	DebugAddWorldArrow( forwardVec, Rgba8::RED, 0.f );


	Vec2 drawSize = m_entityDef->GetDrawSize();
	Vec2 halfSize = drawSize * 0.5f;
	Vec3 rightOffset = Vec3( halfSize.x, 0.f, 0.f );
	Vec3 leftOffset = -rightOffset;
	Vec3 upOffset = Vec3( 0.f, 0.f, drawSize.y );

	Vec3 bottomLeft = m_position + leftOffset;
	Vec3 bottomRight = m_position + rightOffset;
	Vec3 topLeft = bottomLeft + upOffset;
	Vec3 topRight = bottomRight + upOffset;

	
	EntitySpriteAnimStates const* spriteAnimStates = m_entityDef->GetSpriteAnimStates();
	SpriteAnimDefinition const* spriteAnimDef = spriteAnimStates->GetSpriteAnimDefinition( "Walk", Vec2( 1.f, 0.f ) );
	SpriteDefinition const& spriteDef = spriteAnimDef->GetSpriteDefAtTime( 0.f );
	Texture const& spriteTexture = spriteDef.GetTexture();

	AABB2 spriteUVs;
	spriteDef.GetUVs( spriteUVs.mins, spriteUVs.maxs );

	std::vector<Vertex_PCU> vertexList;
	std::vector<uint> indexList;
	Vertex_PCU::AppendVerts4Points( vertexList, indexList, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, spriteUVs );

	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( &spriteTexture );
	g_theRenderer->DrawIndexedVertexArray( vertexList, indexList );


}

