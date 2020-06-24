#include "Game/EntityDefinition.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::map< std::string, EntityDefinition* > EntityDefinition::s_definitions;

EntityDefinition::EntityDefinition( XmlElement const& element )
{
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
}

void EntityDefinition::InitializeEntityDefinitions( XmlElement const& rootEntityDefinitionElement )
{
	std::string rootName = rootEntityDefinitionElement.Name();
	g_theConsole->GuaranteeOrError( rootName == "EntityTypes", Stringf("ERROR: Root name must be [EntityTypes], %s is invalid", rootName.c_str() ) );

	for( XmlElement const* entityTypeElement = rootEntityDefinitionElement.FirstChildElement(); entityTypeElement; entityTypeElement->NextSiblingElement() )
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

