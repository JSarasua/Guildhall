#pragma once
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Tags.hpp"
#include "Game/Map.hpp"

class MapGenStep
{
public:
	MapGenStep( const XMLElement& element );

	static MapGenStep* CreateNewMapGenStep(const XMLElement& element);

	void RunStep( Map& mapToMutate );
	
private:
	virtual void RunStepOnce( Map& mapToMutate ) = 0;

protected:
	float m_chanceToRun = 1.f;
	int m_iterations = 10;
	float m_chanceToMutate = 1.f;
	TileDefinition* m_ifTileType = nullptr;
	TileDefinition* m_changeToTileType = nullptr;
	Tags m_ifTags;
	Tags m_setTags;
	FloatRange m_ifHeatWithin;
	float m_setHeat;
};