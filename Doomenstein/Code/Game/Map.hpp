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
	Map( Game* game );


	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() = 0;

private:
	Game* m_game = nullptr;
};