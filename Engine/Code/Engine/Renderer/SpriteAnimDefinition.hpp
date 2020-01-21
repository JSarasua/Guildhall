#pragma once
#include <vector>
#include "Engine/Core/XmlUtils.hpp"


enum class SpriteAnimPlaybackType
{
	ONCE,		//0,1,2,3,4,4,4,4,4,4
	LOOP,		//0,1,2,3,4,0,1,2,3,4
	PINGPONG	//0,1,2,3,4,3,2,1,0,1
};

class SpriteSheet;
class SpriteDefinition;

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, 
		float durationSeconds, SpriteAnimPlaybackType playbacktype = SpriteAnimPlaybackType::LOOP);

	SpriteAnimDefinition( const SpriteSheet& sheet, const std::vector<int>& spriteIndexes,
		float durationSeconds, SpriteAnimPlaybackType playbacktype = SpriteAnimPlaybackType::LOOP );

	SpriteAnimDefinition( const SpriteSheet& sheet, const XMLElement& element, 
		float framesPerSecond, SpriteAnimPlaybackType playbacktype = SpriteAnimPlaybackType::LOOP );

	const SpriteDefinition& GetSpriteDefAtTime( float seconds ) const;

private:
	const SpriteSheet&		m_spriteSheet;
	std::vector<int>		m_spriteIndexes;
	float					m_durationSeconds	= 1.f;
	SpriteAnimPlaybackType	m_playbackType		= SpriteAnimPlaybackType::LOOP;
	const SpriteDefinition& GetSpriteDefAtTimeOnce( float seconds ) const;
	const SpriteDefinition& GetSpriteDefAtTimeLoop( float seconds ) const;
	const SpriteDefinition& GetSpriteDefAtTimePingPong( float seconds ) const;
};