#include "Map.hpp"
#include "Tile.hpp"
#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;


Map::Map()
{}

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
