#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/Entity.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Portal.hpp"
#include "Game/Projectile.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;


Map::Map( std::string const& name )
{
	m_name = name;
}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees, std::string const& teleporterDestMap, Vec2 const& teleporterDestPos, float teleporterDestYawOffset  )
{
	EntityDefinition const* entityDef = EntityDefinition::GetEntityDefinitionByName( entityDefName );

	return SpawnNewEntityOfType( entityDef, initialPosition, initialPitchRollYawDegrees, teleporterDestMap, teleporterDestPos, teleporterDestYawOffset  );
}

Entity* Map::SpawnNewEntityOfType( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees, std::string const& teleporterDestMap, Vec2 const& teleporterDestPos, float teleporterDestYawOffset )
{
	Entity* newEntity = nullptr;

	if( !entityDef )
	{
		g_theConsole->ErrorString( Stringf("Error: null entity definition" ) );
		return nullptr;
	}

	std::string entityType = entityDef->GetType();

	if( entityType == "Actor" )
	{
		newEntity = new Actor( entityDef, initialPosition, initialPitchRollYawDegrees );
		m_allEntities.push_back( newEntity );
		return newEntity;
	}
	else if( entityType == "Entity" )
	{
		newEntity = new Entity( entityDef, initialPosition, initialPitchRollYawDegrees );
		m_allEntities.push_back( newEntity );
		return newEntity;
	}
	else if( entityType == "Projectile" )
	{
		newEntity = new Projectile( entityDef, initialPosition, initialPitchRollYawDegrees );
		m_allEntities.push_back( newEntity );
		return newEntity;
	}
	else if( entityType == "Portal" )
	{
		newEntity = new Portal( entityDef, initialPosition, initialPitchRollYawDegrees, teleporterDestMap, teleporterDestPos, teleporterDestYawOffset );
		m_allEntities.push_back( newEntity );
		m_portals.push_back( (Portal*)newEntity );
		return newEntity;
	}
	else
	{
		g_theConsole->ErrorString( Stringf("Error: entity type \"%s\" not implemented", entityType.c_str() ) );
		return nullptr;
	}
}

bool Map::IsValid() const
{
	return m_isValid;
}


Entity* Map::GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance  )
{
	Entity* closestEntity = nullptr;
	float initialMaxDistance = maxDistance;
	float currentMaxDistance = maxDistance;
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];
		Vec2 entityPosition = entity->GetPosition();
		Vec3 entity3DPosition = entity->GetEyeHeightPosition();
		
		Vec2 position2D = Vec2( position );
		if( IsPointInForwardSector2D( entityPosition, position2D, forwardVector.GetAngleDegrees(), forwardSpread, currentMaxDistance ) )
		{
			float distanceToEntity = GetDistance3D( entity3DPosition, position );
			float distanceToEntity2D = GetDistance2D( entityPosition, position2D );
			if( distanceToEntity < initialMaxDistance )
			{
				currentMaxDistance = distanceToEntity2D;
				closestEntity = entity;
			}
		}
	}

	return closestEntity;
}

Entity* Map::GetEntityAtImpactPosition( Vec3 const& position, Vec3& surfaceNormal, Entity const* entityToIgnore )
{
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];

		if( entity == entityToIgnore )
		{
			continue;
		}

		if( entity )
		{
			Vec2 position2D = position;

			Vec2 entityPosition = entity->GetPosition();
			float entityRadius = entity->GetPhysicsRadius();
			float entityZHeight = entity->GetHeight();
			float zToCheck = position.z;
			if( zToCheck >= 0.f && zToCheck <= entityZHeight )
			{
				if( IsPointInsideDisc2D( position2D, entityPosition, entityRadius ) )
				{
					if( AlmostEqualsFloat( zToCheck, entityZHeight, 0.005f ) )
					{
						surfaceNormal = Vec3( 0.f, 0.f, 1.f );
						return entity;
					}
					else
					{
						Vec2 surfacNormal2D = position2D - entityPosition;
						surfacNormal2D.Normalize();
						surfaceNormal = surfacNormal2D;
						return entity;
					}
				}
			}


		}

	}

	return nullptr;
}

Map::~Map()
{
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];
		if( entity )
		{
			delete entity;
			m_allEntities[entityIndex] = nullptr;
		}
	}
}

void Map::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	ResolveAllPortalVEntityCollisions();
	ResolveAllEntityVEntityCollisions();
}

void Map::Render()
{
	RenderEntities();
}

void Map::ResolveAllEntityVEntityCollisions()
{
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];
		bool isPushed = entity->IsPushedByEntity();

		if( isPushed )
		{
			ResolveEntityCollisions( entity );
		}
	}
}

void Map::ResolveAllPortalVEntityCollisions()
{
	for( size_t portalIndex = 0; portalIndex < m_portals.size(); portalIndex++ )
	{
		Portal* portal = m_portals[portalIndex];

		if( portal )
		{
			for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
			{
				Entity* entity = m_allEntities[entityIndex];
				if( entity && portal != entity )
				{
					ResolvePortalVEntityCollision( portal, entity );
				}

			}

		}
	}
}

void Map::ResolvePortalVEntityCollision( Portal* portal, Entity* entity )
{
	Vec2 entityPos = entity->GetPosition();
	Vec2 portalPos = portal->GetPosition();

	float entityRadius = entity->GetPhysicsRadius();
	float portalRadius = portal->GetPhysicsRadius();

	bool isOverlap = DoDiscsOverlap( entityPos, entityRadius, portalPos, portalRadius );

	if( isOverlap )
	{
		std::string mapName = portal->GetDestMap();
		if( m_name == mapName )
		{
			Vec3 pitchRollYaw = entity->GetRotationPitchRollYawDegrees();


			entity->SetPosition( portal->GetDestPosition() );
			pitchRollYaw.z += portal->GetDestYawOffset();
			entity->RotatePitchRollYawDegrees( pitchRollYaw );
		}
		else
		{
			if( entity->IsPossessed() )
			{
				//g_theEventSystem->FireEvent( )
			}
		}
	}
}

void Map::ResolveEntityCollisions( Entity* entity )
{
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entityToCheckAgainst = m_allEntities[entityIndex];

		if( entity != entityToCheckAgainst )
		{
			ResolveEntityCollision( entity, entityToCheckAgainst );
		}

	}
}

void Map::ResolveEntityCollision( Entity* entity, Entity* otherEntity )
{
	bool isPushed = entity->IsPushedByEntity();
	bool canPushEntities = entity->CanPushEntity();

	bool otherEntityIsPushed = otherEntity->IsPushedByEntity();
	bool otherEntityCanPush = otherEntity->CanPushEntity();

	Vec2 entityPos = entity->GetPosition();
	Vec2 otherEntityPos = otherEntity->GetPosition();

	float entityRadius = entity->GetPhysicsRadius();
	float otherEntityRadius = otherEntity->GetPhysicsRadius();

	if( !isPushed && !otherEntityIsPushed )
	{
		return;
	}

	if( !canPushEntities && !otherEntityCanPush )
	{
		return;
	}

	if( canPushEntities && otherEntityIsPushed && isPushed && otherEntityCanPush)
	{
		//push both
		PushDiscsOutOfEachOther2D( entityPos, entityRadius, otherEntityPos, otherEntityRadius );
	}
	else if( isPushed && otherEntityCanPush )
	{
		//push self
		PushDiscOutOfDisc2D( entityPos, entityRadius, otherEntityPos, otherEntityRadius );
	}
	else if( canPushEntities && otherEntityIsPushed )
	{
		//push other
		PushDiscOutOfDisc2D( otherEntityPos, otherEntityRadius, entityPos, entityRadius );
	}

	entity->SetPosition( entityPos );
	otherEntity->SetPosition( otherEntityPos );
}

void Map::RenderEntities()
{
	for( size_t entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];
		if( entity )
		{
			entity->Render();
		}
	}
}
