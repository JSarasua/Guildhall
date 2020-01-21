#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XmlUtils.hpp"

class TileDefinition;

class MapGenStep_Mutate : public MapGenStep
{
public:
	MapGenStep_Mutate(const XMLElement& element);

	virtual void RunStepOnce( Map& mapToMutate ) override;

private:
	bool DoTagsAndTilesMatch( TileDefinition* currentTileDef, Tags& currentTags );
};

