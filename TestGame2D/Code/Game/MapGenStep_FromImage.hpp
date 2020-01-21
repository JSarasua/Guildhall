#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Image.hpp"
#include <string>

class TileDefinition;

class MapGenStep_FromImage : public MapGenStep
{
public:
	MapGenStep_FromImage( const XMLElement& element );

	virtual void RunStepOnce( Map& mapToMutate ) override;

protected:
	std::string m_fromImageFilePath;
	IntRange m_rotations;
	FloatRange m_chanceToMirror;
	FloatRange m_uAlignment = FloatRange(0.f,1.f);
	FloatRange m_vAlignment = FloatRange(0.f,1.f);
	Image* m_imageToMap = nullptr;

private:
	IntVec2 GetStartTilePosition( const IntVec2& mapSize, const Vec2& UVs  );

	TileDefinition* GetTileWithMatchingGlyphColorIfValid( const IntVec2& texelCoords );
	std::vector<IntVec2> GetTagTilePositions( Map& mapToMutate );

	void DrawImageAtTagPositions( std::vector<IntVec2>& ifTagTilePositions, Map& mapToMutate );
	void DrawImageAtUVPosition( Map& mapToMutate, Vec2& UVs );
	void DrawImageAtPosition( const IntVec2& positionToDraw, Map& mapToMutate );
};