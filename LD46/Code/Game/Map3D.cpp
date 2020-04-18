#include "Game/Map3D.hpp"
#include "Game/Player3D.hpp"
#include "Game/Companion3D.hpp"

Map3D::Map3D()
{
	m_player = new Player3D();
	m_companion = new Companion3D();
}

Map3D::Map3D( IntVec2 const& mapSize, Game* game ) : 
	m_game( game ),
	m_mapSize( mapSize )
{
	m_player = new Player3D();
	m_companion = new Companion3D();
}

Map3D::~Map3D()
{
	delete m_player;
	m_player = nullptr;

	delete m_companion;
	m_companion = nullptr;

	m_game = nullptr;
}

