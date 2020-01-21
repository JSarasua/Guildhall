#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"



SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbacktype ) :
	m_spriteSheet(sheet),
	m_durationSeconds(durationSeconds),
	m_playbackType(playbacktype)

{
	if( startSpriteIndex == endSpriteIndex )
	{
		m_spriteIndexes.push_back(startSpriteIndex);
		m_spriteIndexes.push_back(startSpriteIndex);
	}
	else if( startSpriteIndex < endSpriteIndex )
	{
		for( int spriteIndex = startSpriteIndex; spriteIndex <= endSpriteIndex; spriteIndex++ )
		{
			m_spriteIndexes.push_back( spriteIndex );
		}
	}
	else if( startSpriteIndex > endSpriteIndex )
	{
		for( int spriteIndex = endSpriteIndex; spriteIndex >= startSpriteIndex; spriteIndex-- )
		{
			m_spriteIndexes.push_back( spriteIndex );
		}
	}
}

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, const std::vector<int>& spriteIndexes, float durationSeconds, SpriteAnimPlaybackType playbacktype ) :
	m_spriteSheet( sheet ),
	m_durationSeconds( durationSeconds ),
	m_playbackType( playbacktype )
{
	m_spriteIndexes = spriteIndexes;
}

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, const XMLElement& element, float framesPerSecond, SpriteAnimPlaybackType playbacktype ) :
	m_spriteSheet( sheet ),
	m_playbackType( playbacktype )
{
	std::string spriteIndexString =  ParseXMLAttribute(element, "spriteIndexes", "");
	float fps = ParseXMLAttribute(element, "fps",-1.f);
	if( fps != -1.f )
	{
		framesPerSecond = fps;
	}

	std::vector<std::string> delimitedStringIndex = SplitStringOnDelimeter(spriteIndexString.c_str(), ',');

	for( int stringIndex = 0; stringIndex < delimitedStringIndex.size(); stringIndex++ )
	{
		const int indexValue = atoi(delimitedStringIndex[stringIndex].c_str());
		m_spriteIndexes.push_back(indexValue);
	}

	m_durationSeconds = static_cast<float>(m_spriteIndexes.size())/framesPerSecond;
}

/****************************************************************************************************/

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{

	switch( m_playbackType )
	{
	case SpriteAnimPlaybackType::ONCE: return GetSpriteDefAtTimeOnce( seconds );
		break;
	case SpriteAnimPlaybackType::LOOP: return GetSpriteDefAtTimeLoop( seconds );
		break;
	case SpriteAnimPlaybackType::PINGPONG: return GetSpriteDefAtTimePingPong( seconds );
		break;
	default:
		ERROR_AND_DIE("Invalid PlaybackType");
		break;
	}

}

/****************************************************************************************************/

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTimeOnce( float seconds ) const
{
	const int numOfFrames = (int)m_spriteIndexes.size();
	const float interval = static_cast<float>(numOfFrames)/m_durationSeconds;


	int index = static_cast<int>(seconds*interval);

	ClampInt(index, 0 , (int)m_spriteIndexes.size());

	return m_spriteSheet.GetSpriteDefinition( m_spriteIndexes[index] );
}

/****************************************************************************************************/

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTimeLoop( float seconds ) const
{
	const int numOfFrames = (int)m_spriteIndexes.size();
	const float interval = static_cast<float>(numOfFrames)/m_durationSeconds;


	int index = static_cast<int>(seconds*interval);

	index = PositiveMod(index, numOfFrames);
	//index += m_startSpriteIndex;

	return m_spriteSheet.GetSpriteDefinition( m_spriteIndexes[index] );
}

/****************************************************************************************************/

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTimePingPong( float seconds ) const
{
	const int numOfFrames = 2 * (int)m_spriteIndexes.size() - 2;
	const int numOfSprites = (int)m_spriteIndexes.size();
	const float interval = static_cast<float>(numOfSprites)/m_durationSeconds;


	int frameNumber = static_cast<int>(seconds*interval);
	frameNumber = PositiveMod(frameNumber, numOfFrames);
	int index = 0;


	if( frameNumber <= numOfFrames/2.f )
	{
		index = frameNumber;
	}
	else
	{
		index = numOfFrames - frameNumber;
	}


	return m_spriteSheet.GetSpriteDefinition( m_spriteIndexes[index] );
}

