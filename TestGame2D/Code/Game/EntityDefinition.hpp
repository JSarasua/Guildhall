#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>
#include "Engine/Math/FloatRange.hpp"

class EntityDefinition
{
	friend class Actor;
	friend class Map;
public:
	EntityDefinition() = delete;
	EntityDefinition( const XmlElement& element );
	~EntityDefinition() {}


	//static void InitializeEntityDefinitions( const XmlElement& rootEntityDefElement );

	//static std::map< std::string, EntityDefinition*> s_definitions;

protected:
	std::string m_name;
	float m_physicsRadius;
	float m_speed;
	float m_turnSpeed;



};