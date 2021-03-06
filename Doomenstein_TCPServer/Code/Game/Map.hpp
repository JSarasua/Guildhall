#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"
#include <string>

class Tile;
class Entity;
class Game;
class Player;
struct Vertex_PCU;
class EntityDefinition;
class Portal;

struct RaycastResult
{
	Entity const* entityToIgnore = nullptr;
	Vec3	startPosition;
	Vec3	forwardNormalOfRaycast;
	float	maxDistance;
	bool	didImpact = false;
	Vec3	impactPosition;
	Entity const* impactEntity = nullptr;
	float	impactFraction = 0.f;
	float	impactDistance = 0.f;
	Vec3	impactSurfaceNormal;

};

class Map
{
public:
	Map() {}
	Map( std::string const& name );
	virtual ~Map();


	virtual void Update( float deltaSeconds );
	virtual void Render();

	virtual void SetPlayerToStart() = 0;
	virtual Vec2 const& GetPlayerStartPosition() = 0;
	virtual float GetPlayerStartYaw() = 0;

	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees, std::string const& teleporterDestMap = std::string(), Vec2 const& teleporterDestPos = Vec2(), float teleporterDestYawOffset = 0.f  );
	virtual Entity* SpawnNewEntityOfType( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& initialPitchRollYawDegrees, std::string const& teleporterDestMap, Vec2 const& teleporterDestPos, float teleporterDestYawOffset  );

	bool IsValid() const;

	Entity* GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance );
	Entity* GetEntityAtImpactPosition( Vec3 const& position, Vec3& surfaceNormal, Entity const* entityToIgnore );

	virtual RaycastResult Raycast( Vec3 const& startPosition, Vec3 const& forwardNormal, float maxDistance, Entity const* entityToIgnore ) = 0;

	//Warping helper methods
	void RemoveEntity( Entity* entity );
	void AddEntity( Entity* entity );
private:
	void ResolveAllEntityVEntityCollisions();
	void ResolveAllPortalVEntityCollisions();
	void ResolvePortalVEntityCollision( Portal* portal, Entity* entity );
	void ResolveEntityCollisions( Entity* entity );
	void ResolveEntityCollision( Entity* entity, Entity* otherEntity );
	void RenderEntities();

protected:
	bool m_isValid = false;

	std::vector<Entity*> m_allEntities;
	std::vector<Portal*> m_portals;

	RaycastResult m_currentRaycastResult;

private:


public:
	std::string m_name;
};