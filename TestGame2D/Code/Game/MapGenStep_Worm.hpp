#pragma once

#include "Game/MapGenStep.hpp"
#include "Engine/Core/XmlUtils.hpp"


class TileDefinition;

class MapGenStep_Worm : public MapGenStep
{
public:
	MapGenStep_Worm( const XMLElement& element );

	virtual void RunStepOnce( Map& mapToMutate ) override;


protected:
	int m_wormLength = 0;
	TileDefinition* m_changeToTileType = nullptr;


};