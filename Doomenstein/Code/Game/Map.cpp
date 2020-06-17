#include "Map.hpp"
#include "Tile.hpp"
#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
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

Map::Map(Game* game) :
	m_game(game)
{
}

bool Map::IsValid() const
{
	return m_isValid;
}

Map::~Map()
{

}