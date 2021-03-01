#pragma once
#include "Engine/Core/Rgba8.hpp"
#include <string>

class Texture;

struct UIState
{
public:
	Texture* m_texture = nullptr;
	Texture* m_backgroundTexture = nullptr;
	std::string m_text;
	float m_textSize = 0.f;
	Rgba8 m_tint = Rgba8::WHITE;
};