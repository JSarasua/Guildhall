#include "Engine/Renderer/SpriteAnimSet.hpp"

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"

SpriteAnimSet::SpriteAnimSet( const XmlElement& element, SpriteSheet& spriteSheet )
{

	float framesPerSecond = ParseXMLAttribute(element,"fps",15.f);

	for( const XmlElement* currentElement = element.FirstChildElement(); currentElement; currentElement=currentElement->NextSiblingElement() ) {
		//std::string attributeName = currentElement->Name();
		std::string animationName = ParseXMLAttribute(*currentElement, "name", std::string(""));
		SpriteAnimDefinition* spriteAnimDef = new SpriteAnimDefinition(spriteSheet,*currentElement,framesPerSecond, SpriteAnimPlaybackType::PINGPONG);

		m_spriteAnimations[animationName] = spriteAnimDef;
	}
}

SpriteAnimDefinition* SpriteAnimSet::GetSpriteAnimDefinition( const std::string& animName ) const
{
	SpriteAnimConstIterator spriteAnimIterator = m_spriteAnimations.find(animName);
	if( spriteAnimIterator != m_spriteAnimations.cend() )
	{
		return spriteAnimIterator->second;
	}

	return nullptr;
}

