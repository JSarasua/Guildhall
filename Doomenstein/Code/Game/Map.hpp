#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"


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


	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() = 0;

	virtual void SetPlayerToStart() = 0;
	virtual Vec2 const& GetPlayerStartPosition() = 0;
	virtual float GetPlayerStartYaw() = 0;

	bool IsValid() const;

protected:
	bool m_isValid = false;

private:
};