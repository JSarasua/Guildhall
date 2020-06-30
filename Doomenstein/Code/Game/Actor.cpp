#include "Game/Actor.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"

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



	std::vector<Vec3> vertsCounterClockwise = GetBillboardedVertsCounterClockwise( m_entityDef->GetBillboardType() );
	EntitySpriteAnimStates const* spriteAnimStates = m_entityDef->GetSpriteAnimStates();
	Vec2 localDirToCamera = GetLocalDirectionToMainCamera();
	SpriteAnimDefinition const* spriteAnimDef = spriteAnimStates->GetSpriteAnimDefinition( "Walk", localDirToCamera );
	SpriteDefinition const& spriteDef = spriteAnimDef->GetSpriteDefAtTime( 0.f );
	Texture const& spriteTexture = spriteDef.GetTexture();

	AABB2 spriteUVs;
	spriteDef.GetUVs( spriteUVs.mins, spriteUVs.maxs );

	std::vector<Vertex_PCU> vertexList;
	std::vector<uint> indexList;
	Vertex_PCU::AppendVerts4Points( vertexList, indexList, vertsCounterClockwise[0], vertsCounterClockwise[1], vertsCounterClockwise[2], vertsCounterClockwise[3], Rgba8::WHITE, spriteUVs );

	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( &spriteTexture );
	g_theRenderer->DrawIndexedVertexArray( vertexList, indexList );


}

std::vector<Vec3> Actor::GetBillboardedVertsCounterClockwise( std::string const& billboardType ) const
{
	Vec2 drawSize = m_entityDef->GetDrawSize();
	Vec2 halfSize = drawSize * 0.5f;

	Vec3 centerPos = Vec3( m_position, halfSize.y );
	Vec3 rightOffset = Vec3( halfSize.x, 0.f, 0.f );
	Vec3 leftOffset = -rightOffset;
	Vec3 upOffset = Vec3( 0.f, 0.f, halfSize.y );
	Vec3 downOffset = -upOffset;

	if( billboardType == "CameraFacingXY" )
	{
		//Normal = forward vector
		//Left = Normal rotated 90 (-y,x)
		//Right = -Left
		//Up (0,0,1)
		Vec2 cameraPos = g_theGame->GetCameraPostion();
		Vec2 normal = cameraPos - m_position;
		normal.Normalize();
		Vec2 leftDir = normal.GetRotatedMinus90Degrees();
		Vec2 rightDir = -leftDir;
		Vec3 upDir = Vec3( 0.f, 0.f, 1.f );
		Vec3 downDir = Vec3( 0.f, 0.f, -1.f );

		leftOffset = leftDir * halfSize.x;
		rightOffset = rightDir * halfSize.x;
		upOffset = upDir * halfSize.y;
		downOffset = downDir * halfSize.y;
	}
	else if( billboardType == "CameraOpposingXY" )
	{
		Vec2 normal = g_theGame->GetCameraForward();
		normal *= -1.f;
		normal.Normalize();
		Vec2 leftDir = normal.GetRotatedMinus90Degrees();
		Vec2 rightDir = -leftDir;
		Vec3 upDir = Vec3( 0.f, 0.f, 1.f );
		Vec3 downDir = Vec3( 0.f, 0.f, -1.f );

		leftOffset = leftDir * halfSize.x;
		rightOffset = rightDir * halfSize.x;
		upOffset = upDir * halfSize.y;
		downOffset = downDir * halfSize.y;
	}
	else if( billboardType == "CameraFacingXYZ" )
	{
		Vec3 cameraPos = g_theGame->GetCameraPostion();
		Vec3 normal = cameraPos - m_position;
		normal.Normalize();
		Vec3 leftDir = Vec3( normal.y, -normal.x, 0.f );
		leftDir.Normalize();
		Vec3 rightDir = -leftDir;
		Vec3 upDir = CrossProduct( normal, rightDir );
		Vec3 downDir = -upDir;

		leftOffset = leftDir * halfSize.x;
		rightOffset = rightDir * halfSize.x;
		upOffset = upDir * halfSize.y;
		downOffset = downDir * halfSize.y;
	}
	else if( billboardType == "CameraOpposingXYZ" )
	{
		Mat44 cameraModel = g_theGame->GetCameraModelMatrix();
		Vec3 normal = cameraModel.GetIBasis3D();
		normal *= -1.f;
		Vec3 leftDir = cameraModel.GetJBasis3D();
		Vec3 rightDir = -leftDir;
		Vec3 upDir = cameraModel.GetKBasis3D();
		Vec3 downDir = -upDir;

		rightOffset = rightDir * halfSize.x;
		leftOffset = leftDir * halfSize.x;
		upOffset = upDir * halfSize.y;
		downOffset = downDir * halfSize.y;
	}

	Vec3 bottomLeft = centerPos + leftOffset + downOffset;
	Vec3 bottomRight = centerPos + rightOffset + downOffset;
	Vec3 topLeft = centerPos + leftOffset + upOffset;
	Vec3 topRight = centerPos + rightOffset + upOffset;

	std::vector<Vec3> verts;
	verts.push_back( bottomLeft );
	verts.push_back( bottomRight );
	verts.push_back( topRight );
	verts.push_back( topLeft );

	return verts;
}

Vec2 Actor::GetLocalDirectionToMainCamera() const
{
	Vec2 cameraPos = g_theGame->GetCameraPostion();
	Vec2 normal = cameraPos - m_position;
	normal.Normalize();

	Vec2 forward = GetForwardVector();
	forward.Normalize();
	Vec2 leftDir = forward.GetRotated90Degrees();

	Vec2 localDirectionToMainCamera;
	localDirectionToMainCamera.x = DotProduct2D( normal, forward );
	localDirectionToMainCamera.y = DotProduct2D( normal, leftDir );

	return localDirectionToMainCamera;
}

