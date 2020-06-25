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
	ResolveEntityCollisions();
}

void Map::Render()
{
	RenderEntities();
}

void Map::ResolveEntityCollisions()
{

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
