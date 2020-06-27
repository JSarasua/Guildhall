#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"


class Tile;
class Entity;
class Game;
class Player;
struct Vertex_PCU;

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

	bool IsValid() const;

	Entity* GetClosestEntityInSector( Vec3 const& position, Vec2 const& forwardVector, float forwardSpread, float maxDistance );

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