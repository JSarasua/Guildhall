#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>
#include "Engine/Audio/AudioSystem.hpp"

class AudioDefinition
{
public:
	AudioDefinition() = delete;
	AudioDefinition( XmlElement const& element );
	~AudioDefinition() {}

	static void InitializeAudioDefinitions( XmlElement const& rootAudioElement );
	static AudioDefinition* GetAudioDefinition( std::string soundName );
	static void StopAllSounds();
	static void UpdateAllVolumes();
	static void ChangeMasterVolume( float newMasterVolume );
	static void ChangeBackgroundMusicVolume( float newBackgroundMusicVolume );
	static void ChangeSFXVolumme( float newSFXVolume );

	static std::map< std::string, AudioDefinition*> s_definitions;

	static float s_masterVolume;
	static float s_backgroundMusicVolume;
	static float s_SFXVolume;

	void PlaySound();
	void StopSound();
	void UpdateVolume();
protected:
	SoundID m_soundID;
	bool m_isLooped = false;
	bool m_isBackgroundMusic = false;
	bool m_isSFX = false;

	SoundPlaybackID m_soundPlaybackID;
	bool m_isPlaying = false;
};