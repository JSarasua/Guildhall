#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>



class EntityDefinition
{
public:
	EntityDefinition() = delete;
	EntityDefinition( XmlElement const& element );
	~EntityDefinition() {}

	static void InitializeEntityDefinitions( XmlElement const& rootEntityDefinitionElement );
	static std::map< std::string, EntityDefinition* > s_definitions;



protected:
	std::string m_name;
	std::string m_type;
	bool m_isValid = false;

	bool m_canBePushedByWalls = false;
	bool m_canBePushedByEntities = false;
	bool m_canPushEntities = false;
	float m_mass = 1.f;
	float m_height = 0.f;
	float m_eyeHeight = 0.f;
	float m_physicsRadius = 0.f;
	float m_walkSpeed = 0.f;

};