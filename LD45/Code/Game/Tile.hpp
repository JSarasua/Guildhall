#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"

class Game;

enum TileType
{
	INVALID_TILETYPE = -1,

	SOLID,
	VISIBLESOLID,
	NOTSOLID,
	INVISIBLELEFT,
	VISIBLELEFT,
	INVISIBLERIGHT,
	VISIBLERIGHT,
	INVISIBLEUP,
	VISIBLEUP,
	INVISIBLEDOWN,
	VISIBLEDOWN,
	VICTORY,
	START,
	DEATH,


	NUM_TILETYPES
};

class Tile
{
public:
	Tile() {}
	~Tile(){}

	Tile(const IntVec2& tileCoords, const AABB2& tileSize, TileType tileType, int tileIndex, Game* game);

	void Update(float deltaSeconds);
	void Render() const;


	AABB2 GetAABB2() const;
	bool isSolid() const;
	void setHit(int playerID);
	bool isVictory() const;
	void SetTileType(TileType newTileType);
	TileType getTileType() const;
	bool WasJustHit(int playerID) const;
	void setFlutter();
	void setNotHit(int playerID);
private:
	IntVec2 m_tileCoords;
	AABB2 m_tileSize;
	TileType m_tileType = INVALID_TILETYPE;
	int m_tileIndex = 0;
	bool m_isHit[4] = {false, false, false, false};
	bool m_wasHit[4] = {false, false, false, false};
	bool m_isFlusttering = false;
	Game* m_game = nullptr;
	unsigned char m_flutter = 0;
	float m_timer = 5.f;
};