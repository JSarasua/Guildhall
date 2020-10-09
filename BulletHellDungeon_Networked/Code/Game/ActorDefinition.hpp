#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/EntityDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"


class ActorDefinition : public EntityDefinition
{
	friend class Actor;
	friend class Map;
public:
	ActorDefinition() = delete;
	ActorDefinition( const XmlElement& element );
	~ActorDefinition() {}

	static void InitializeActorDefinitions( const XmlElement& rootActorDefElement );

	static std::map< std::string, ActorDefinition*> s_definitions;

protected:
	//std::string m_name;
	AABB2 m_spriteUVs;
	Rgba8 m_tint;
	AABB2 m_drawBounds;
	AABB2 m_spritePortraitUVs;
	//float m_physicsRadius = 1000.f;
	//float m_turnSpeed;
	//float m_speed;
	bool m_canWalk = false;
	bool m_canFly = false;
	bool m_canSwim = false;
	FloatRange m_waitingTime = FloatRange(3.f,5.f);
	SpriteAnimSet* m_actorAnimations = nullptr;
};