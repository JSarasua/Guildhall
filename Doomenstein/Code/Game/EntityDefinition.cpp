#include "Game/EntityDefinition.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::map< std::string, EntityDefinition* > EntityDefinition::s_definitions;

EntityDefinition::EntityDefinition( XmlElement const& element )
{
	m_type = element.Name();
	g_theConsole->GuaranteeOrError( m_type == "Actor" || m_type == "Entity" || m_type == "Projectile" || m_type == "Portal", "Invalid Entity Type" );

	XmlElement const* physicsElement = element.FirstChildElement( "Physics" );
	if( physicsElement )
	{
		m_physicsRadius = ParseXMLAttribute( *physicsElement, "radius", 0.1f );
		m_height = ParseXMLAttribute( *physicsElement, "height", 0.75f );
		m_eyeHeight = ParseXMLAttribute( *physicsElement, "eyeHeight", 0.5f );
		m_walkSpeed = ParseXMLAttribute( *physicsElement, "walkSpeed", 1.f );
		m_canBePushedByWalls = true;
		m_canBePushedByEntities = true;
		m_canPushEntities = true;
		m_mass = 1.f;
	}


	XmlElement const* appearanceElement = element.FirstChildElement( "Appearance" );
	UNUSED( appearanceElement );
	// 	std::string m_name;
// 	std::string m_type;
// 	bool m_isValid = false;
// 
// 	bool m_canBePushedByWalls = false;
// 	bool m_canBePushedByEntities = false;
// 	bool m_canPushEntities = false;
// 	float m_mass = 1.f;
// 	float m_height = 0.f;
// 	float m_eyeHeight = 0.f;
// 	float m_physicsRadius = 0.f;
// 	float m_walkSpeed = 0.f;
	m_isValid = true;
}

void EntityDefinition::InitializeEntityDefinitions( XmlElement const& rootEntityDefinitionElement )
{
	std::string rootName = rootEntityDefinitionElement.Name();
	g_theConsole->GuaranteeOrError( rootName == "EntityTypes", Stringf("ERROR: Root name must be [EntityTypes], %s is invalid", rootName.c_str() ) );

	for( XmlElement const* entityTypeElement = rootEntityDefinitionElement.FirstChildElement(); entityTypeElement; entityTypeElement = entityTypeElement->NextSiblingElement() )
	{
		XmlAttribute const* entityTypeNameAttribute = entityTypeElement->FindAttribute( "name" );
		if( entityTypeNameAttribute )
		{
			std::string entityTypeName = entityTypeNameAttribute->Value();
			EntityDefinition* entityDef = new EntityDefinition( *entityTypeElement );

			if( entityDef->m_isValid )
			{
				s_definitions[entityTypeName] = entityDef;
			}
			else
			{
				delete entityDef;
				entityDef = nullptr;
			}
		}
		else
		{
			g_theConsole->ErrorString( "Missing name for EntityType" );
		}
	}
}

