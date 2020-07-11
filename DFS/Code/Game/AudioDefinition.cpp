#include "Game/AudioDefinition.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, AudioDefinition*> AudioDefinition::s_definitions;

float AudioDefinition::s_masterVolume = 1.f;
float AudioDefinition::s_backgroundMusicVolume = 1.f;
float AudioDefinition::s_SFXVolume = 1.f;

void AudioDefinition::PlaySound()
{
	float bgMusicVolume = 1.f;
	float sfxVolume = 1.f;

	if( m_isBackgroundMusic )
	{
		bgMusicVolume = s_backgroundMusicVolume;
	}

	if( m_isSFX )
	{
		sfxVolume = s_SFXVolume;
	}

	float currentVolume = s_masterVolume * bgMusicVolume * sfxVolume;
	m_soundPlaybackID = g_theAudio->PlaySound( m_soundID, m_isLooped, currentVolume );
	m_isPlaying = true;

	
}

void AudioDefinition::StopSound()
{
	if( m_isPlaying )
	{
		g_theAudio->StopSound( m_soundPlaybackID );
	}

	m_isPlaying = false;
}

AudioDefinition::AudioDefinition( XmlElement const& element )
{
	std::string soundStringFile = "Data/Audio/";
	std::string soundFile = ParseXMLAttribute( element, "Sound", "INVALID" );
	soundStringFile.append( soundFile );
	m_soundID = g_theAudio->CreateOrGetSound( soundStringFile );

	m_isLooped = ParseXMLAttribute( element, "IsLooped", false );
	m_isBackgroundMusic = ParseXMLAttribute( element, "IsBackgroundMuisc", false );
	m_isSFX = ParseXMLAttribute( element, "IsSFX", false );
}

void AudioDefinition::InitializeAudioDefinitions( XmlElement const& rootAudioElement )
{
	for( XmlElement const* element = rootAudioElement.FirstChildElement(); element; element = element->NextSiblingElement() )
	{
		std::string name = ParseXMLAttribute( *element, "Name", "INVALID" );

		AudioDefinition* audioDef = new AudioDefinition( *element );
		s_definitions[name] = audioDef;
	}
}

AudioDefinition* AudioDefinition::GetAudioDefinition( std::string soundName )
{
	return s_definitions[soundName];
}

void AudioDefinition::StopAllSounds()
{
	for( auto audioIter = s_definitions.begin(); audioIter != s_definitions.end(); audioIter++ )
	{
		audioIter->second->StopSound();
	}
}

