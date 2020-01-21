#include "Game/NpcTank.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



extern App* g_theApp;
extern RenderContext* g_theRenderer;


NpcTank::NpcTank( const Vec2& initialPosition, Game* game ):
	Entity( game )
{
	m_position = initialPosition;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
}

NpcTank::NpcTank( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game ):
	Entity( type, faction, game )
{
	m_position = initialPosition;
	m_physicsRadius = NPCTANK_PHYSICS_RADIUS;
	m_cosmeticRadius = NPCTANK_COSMETIC_RADIUS;
	m_orientationDegrees = orientationDegrees;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, NPCTANK_SPEED );

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = NPCTANK_HEALTH;
}

void NpcTank::Startup()
{

}

void NpcTank::Update( float deltaSeconds )
{
	if( m_currentState == WANDERING )
	{
		UpdateWander(deltaSeconds);
	}
	else if( m_currentState == FINDING )
	{
		UpdateFind(deltaSeconds);
	}
	else if( m_currentState == PURSUING )
	{
		UpdatePursue(deltaSeconds);
	}
	else
	{
		//SHOULDN'T BE HERE
		ERROR_AND_DIE("NpcTank has an invalid state");
	}

	m_velocity *= m_velocityScale;

	m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
	//Entity::Update(deltaSeconds);
}

void NpcTank::Render() const
{
	AABB2 npcTankAABB2 = AABB2( Vec2( -m_cosmeticRadius, -m_cosmeticRadius ), Vec2( m_cosmeticRadius, m_cosmeticRadius ) );
	Vertex_PCU npcTankVertexes[] =
	{
		Vertex_PCU( Vec3( npcTankAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTankAABB2.maxs.x, npcTankAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( npcTankAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( npcTankAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( npcTankAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2( npcTankAABB2.mins.x, npcTankAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

	};


	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank4.png" );
	Vertex_PCU::TransformVertexArray( 6, npcTankVertexes, 1.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture( baseTexture );
	g_theRenderer->DrawVertexArray( 6, npcTankVertexes );



	Entity::DebugRender();
}

Vec2 NpcTank::GetNosePosition() const
{
	Vec2 noseVec = Vec2::MakeFromPolarDegrees( m_orientationDegrees );
	noseVec *= m_physicsRadius;
	noseVec += m_position;
	return noseVec;
}

void NpcTank::UpdateWander( float deltaSeconds )
{
	static float goalOrientation = m_game->m_rand.RollRandomFloatInRange( -180.f, 180.f );
	static float timer = static_cast<float>(GetCurrentTimeSeconds());
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());

	if( currentTime > timer + 2.f )
	{
		timer = currentTime;
		goalOrientation = m_game->m_rand.RollRandomFloatInRange( -180.f, 180.f );
	}

	float goalOrientationAvoidingWalls = AvoidWalls(goalOrientation);



	m_orientationDegrees = GetTurnedToward( m_orientationDegrees, goalOrientationAvoidingWalls, NPCTANK_ANGULARVELOCITY * deltaSeconds );
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, NPCTANK_SPEED );


	m_velocity *= m_velocityScale;


	if( m_wallCollisionState != NOTCOLLIDING )
	{
		//timer = currentTime;
		if( m_wallCollisionState == LEFTWHISKERCOLLIDING )
		{
			goalOrientation -= 10.f*deltaSeconds;
		}
		else if( m_wallCollisionState == RIGHTWHISKERCOLLIDING )
		{
			goalOrientation += 10.f*deltaSeconds;
		}
		else if( m_wallCollisionState == BOTHWHISKERSCOLLIDING )
		{
			goalOrientation += 10.f*deltaSeconds;
		}
	}

}

void NpcTank::UpdateFind( float deltaSeconds )
{
	if( DoDiscsOverlap( m_EnemyPosition, 0.f, m_position, m_physicsRadius ) )
	{
		m_currentState = WANDERING;
		return;
	}

	float enemyAngle = (m_EnemyPosition - m_position).GetAngleDegrees();
	float enemyAngleAvoidingWalls = AvoidWalls(enemyAngle);

	m_orientationDegrees = GetTurnedToward( m_orientationDegrees, enemyAngleAvoidingWalls, NPCTANK_ANGULARVELOCITY * deltaSeconds );


// 	if( absFloat(GetShortestAngularDisplacement( m_orientationDegrees, enemyAngleAvoidingWalls )) < 20.f )
// 	{
		m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, NPCTANK_SPEED );
// 	}
// 	else
// 	{
// 		m_velocity *= 0.f;
// 	}

	m_velocity *= m_velocityScale;


}

void NpcTank::UpdatePursue( float deltaSeconds )
{
	float enemyAngle = (m_EnemyPosition - m_position).GetAngleDegrees();
	float enemyAngleAvoidingWalls = AvoidWalls(enemyAngle);

	m_orientationDegrees = GetTurnedToward( m_orientationDegrees, enemyAngleAvoidingWalls, NPCTANK_ANGULARVELOCITY * deltaSeconds );


	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, NPCTANK_SPEED );

	if( !m_CanDriveTowardsEnemy )
	{
		m_velocity *= 0.3f;
	}


	m_velocity *= m_velocityScale;

}

float NpcTank::AvoidWalls( float goalOrientation )
{
	if( m_middleWhisker > NPC_TANK_MIDDLEWHISKERRANGE - 0.001f )
	{
		m_UseMiddleWhisker = false;
	}

	if( m_leftWhisker < NPCTANK_WHISKERRANGE - 0.001f && m_rightWhisker < NPCTANK_WHISKERRANGE - 0.001f )
	{
		m_wallCollisionState = BOTHWHISKERSCOLLIDING;
		m_UseMiddleWhisker = true;
	}
	else if( m_leftWhisker < NPCTANK_WHISKERRANGE - 0.001f )
	{
		m_wallCollisionState = LEFTWHISKERCOLLIDING;
	}
	else if( m_rightWhisker < NPCTANK_WHISKERRANGE - 0.001f )
	{
		m_wallCollisionState = RIGHTWHISKERCOLLIDING;
	}
	else
	{
		m_wallCollisionState = NOTCOLLIDING;
	}

	switch( m_wallCollisionState )
	{
	case NOTCOLLIDING:
		m_velocityScale = 1.f;
		return goalOrientation;
		break;
	case LEFTWHISKERCOLLIDING: 
		if( m_UseMiddleWhisker )
		{
			return SteerLeft(goalOrientation);
		}
		else
		{
			return SteerRight(goalOrientation);
		}
		break;
	case RIGHTWHISKERCOLLIDING: return SteerLeft(goalOrientation);
		break;
	case BOTHWHISKERSCOLLIDING: return SteerLeft(goalOrientation);
		break;
	default:
		return goalOrientation;
		break;
	}


}

float NpcTank::SteerLeft( float goalOrientation )
{
	float haltingDistance = NPCTANK_WHISKERRANGE*0.5f;
	//float whiskerScale = (m_rightWhisker-haltingDistance) / (NPCTANK_WHISKERRANGE-haltingDistance);
	m_velocityScale = RangeMap(haltingDistance, NPCTANK_WHISKERRANGE, 0.5f, 1.f, m_rightWhisker);
	float goalOrientationScale = RangeMap(haltingDistance, NPCTANK_WHISKERRANGE, 0.5f, 1.f, m_rightWhisker);




	return goalOrientation + 90.f*(1-goalOrientationScale); //Increase goal to keep it rotating left
}

float NpcTank::SteerRight( float goalOrientation )
{
	float haltingDistance = NPCTANK_WHISKERRANGE*0.5f;
	//float whiskerScale = (m_leftWhisker-haltingDistance) / (NPCTANK_WHISKERRANGE-haltingDistance);
	m_velocityScale = RangeMap( haltingDistance, NPCTANK_WHISKERRANGE, 0.5f, 1.f, m_leftWhisker );
	float goalOrientationScale = RangeMap(haltingDistance, NPCTANK_WHISKERRANGE, 0.5f, 1.f, m_leftWhisker);


	return goalOrientation - 90.f*(1-goalOrientationScale); //Increase goal to keep it rotating right
}

