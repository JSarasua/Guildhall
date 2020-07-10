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
	static std::map< std::string, AudioDefinition*> s_definitions;

	static float s_masterVolume;
	static float s_backgroundMusicVolume;
	static float s_SFXVolume;

	void PlaySound();
protected:
	SoundID m_soundID;
	bool m_isLooped = false;
};