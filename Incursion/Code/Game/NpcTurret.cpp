#include "Game/Npcturret.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"



extern App* g_theApp;
extern RenderContext* g_theRenderer;



NpcTurret::NpcTurret( const Vec2& initialPosition, Game* game ):
	Entity( game )
{
	m_position = initialPosition;

	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = NPCTURRET_HEALTH;
}

NpcTurret::NpcTurret( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game ):
	Entity( type, faction, game )
{
	m_position = initialPosition;
	m_physicsRadius = NPCTANK_PHYSICS_RADIUS;
	m_cosmeticRadius = NPCTANK_COSMETIC_RADIUS;
	m_orientationDegrees = orientationDegrees;
	m_angularVelocity = NPCTURRET_ANGULARVELOCITY;

	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = NPCTURRET_HEALTH;
}

void NpcTurret::Startup()
{

}

void NpcTurret::Update( float deltaSeconds )
{
	if( !m_HasSeenEnemy )
	{
		Entity::Update(deltaSeconds);
	}
	else if( !m_DoesSeeEnemy )
	{

		//const Vec2 displacementVec = m_EnemyPosition - m_position;
		//const float angleTowardsEnemy = displacementVec.GetAngleDegrees();
		m_orientationDegrees = GetTurnedToward( m_orientationDegrees, m_currentGoalAngle, m_angularVelocity*deltaSeconds );

		SetNewGoalOrientation();

	}
	else
	{
		const Vec2 displacementVec = m_EnemyPosition - m_position;
		m_angleTowardsEnemy = displacementVec.GetAngleDegrees();
		m_orientationDegrees = GetTurnedToward(m_orientationDegrees, m_angleTowardsEnemy, m_angularVelocity*deltaSeconds);
		m_currentGoalAngle = m_angleTowardsEnemy;

		m_IsLookingForEnemy = false;
	}

}

void NpcTurret::Render() const
{
	AABB2 npcTurretBaseAABB2 = AABB2( Vec2( -m_cosmeticRadius, -m_cosmeticRadius ), Vec2( m_cosmeticRadius, m_cosmeticRadius ) );
	Vertex_PCU npcTurretBaseVertexes[] =
	{
		Vertex_PCU( Vec3( npcTurretBaseAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTurretBaseAABB2.maxs.x, npcTurretBaseAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( npcTurretBaseAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( npcTurretBaseAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( npcTurretBaseAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTurretBaseAABB2.mins.x, npcTurretBaseAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

	};


	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	Vertex_PCU::TransformVertexArray( 6, npcTurretBaseVertexes, 1.f, 0.f, m_position );
	g_theRenderer->BindTexture( baseTexture );
	g_theRenderer->DrawVertexArray( 6, npcTurretBaseVertexes );





	AABB2 npcTurretAABB2 = AABB2( Vec2( -m_cosmeticRadius, -m_cosmeticRadius ), Vec2( m_cosmeticRadius, m_cosmeticRadius ) );
	Vertex_PCU npcTurretVertexes[] =
	{
		Vertex_PCU( Vec3( npcTurretAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTurretAABB2.maxs.x, npcTurretAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( npcTurretAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( npcTurretAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( npcTurretAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTurretAABB2.mins.x, npcTurretAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

	};


	Texture* turretTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	Vertex_PCU::TransformVertexArray( 6, npcTurretVertexes, 1.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture( turretTexture );
	g_theRenderer->DrawVertexArray( 6, npcTurretVertexes );



	Entity::DebugRender();
}

Vec2 NpcTurret::GetNosePosition() const
{
	Vec2 noseVec = Vec2::MakeFromPolarDegrees( m_orientationDegrees );
	noseVec *= m_physicsRadius;
	noseVec += m_position;
	return noseVec;
}

void NpcTurret::SetNewGoalOrientation()
{
	const float halfFOV = NPCTURRET_APERTURE_DEGREES * 0.5f;

	if( m_IsLookingForEnemy )
	{
		if( absFloat( GetShortestAngularDisplacement( m_orientationDegrees, m_angleTowardsEnemy + halfFOV ) ) < 1.f )
		{
			m_currentGoalAngle = m_angleTowardsEnemy - halfFOV;
		}
		else if( absFloat( GetShortestAngularDisplacement( m_orientationDegrees, m_angleTowardsEnemy - halfFOV ) ) < 1.f )
		{
			m_currentGoalAngle = m_angleTowardsEnemy + halfFOV;
		}
	}
	else
	{
			m_currentGoalAngle = m_angleTowardsEnemy + halfFOV;
			m_IsLookingForEnemy = true;
	}

}
