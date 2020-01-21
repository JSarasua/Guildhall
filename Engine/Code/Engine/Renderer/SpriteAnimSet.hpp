#pragma once
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"

typedef std::map<std::string, SpriteAnimDefinition*>::const_iterator SpriteAnimConstIterator;


class SpriteAnimSet
{
public:
	SpriteAnimSet( const XMLElement& element, SpriteSheet& spriteSheet );

	SpriteAnimDefinition* GetSpriteAnimDefinition(const std::string& animName) const;

private:
	std::map<std::string, SpriteAnimDefinition*> m_spriteAnimations;
};