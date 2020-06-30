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


Map::Map()
{}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees )
{
	EntityDefinition const* entityDef = EntityDefinition::GetEntityDefinitionByName( entityDefName );

	return SpawnNewEntityOfType( entityDef, initialPosition, initialPitchRollYawDegrees );
}

Entity* Map::SpawnNewEntityOfType( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees )
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
		newEntity = new Portal( entityDef, initialPosition, initialPitchRollYawDegrees );
		m_allEntities.push_back( newEntity );
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
