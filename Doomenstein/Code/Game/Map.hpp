#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <string>

class Tile;
class Entity;
class Game;
class Player;
struct Vertex_PCU;
class EntityDefinition;

struct RaycastResult
{
	Entity* entityToIgnore = nullptr;
	Vec3	startPosition;
	Vec3	forwardNormalOfRaycast;
	float	maxDistance;
	bool	didImpact = false;
	Vec3	impactPosition;
	Entity* impactEntity = nullptr;
	float	impactFraction = 0.f;
	float	impactDistance = 0.f;
	Vec3	impactSurfaceNormal;

};

class Map
{
public:
	Map();
	virtual ~Map();


	virtual void Update( float deltaSeconds );
	virtual void Render();

	virtual void SetPlayerToStart() = 0;
	virtual Vec2 const& GetPlayerStartPosition() = 0;
	virtual float GetPlayerStartYaw() = 0;

	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees );
	virtual Entity* SpawnNewEntityOfType( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees );

	bool IsValid() const;

	Entity* GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance );

	virtual RaycastResult Raycast( Vec3 const& startPosition, Vec3 const& forwardNormal, float maxDistance, Entity* entityToIgnore ) = 0;

private:
	void ResolveAllEntityVEntityCollisions();
	void ResolveEntityCollisions( Entity* entity );
	void ResolveEntityCollision( Entity* entity, Entity* otherEntity );
	void RenderEntities();

protected:
	bool m_isValid = false;

	std::vector<Entity*> m_allEntities;

private:
};