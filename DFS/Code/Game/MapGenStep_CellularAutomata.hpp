#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XmlUtils.hpp"


class TileDefinition;

class MapGenStep_CellularAutomata : public MapGenStep
{
public:
	MapGenStep_CellularAutomata( const XmlElement& element );

	virtual void RunStepOnce(  Map& mapToMutate ) override;

private:
	bool MetNeighborTileRequirement( Tile& currentTile, Map& mapToMutate );

protected:
	TileDefinition* m_ifNeighborTileType = nullptr;
	IntRange m_numOfNeighbors;
};