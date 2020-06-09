#include "Game/EntityDefinition.hpp"


EntityDefinition::EntityDefinition( const XmlElement& element )
{
	m_name						= ParseXMLAttribute( element, "name", "INVALID");
	m_physicsRadius				= ParseXMLAttribute( element, "physicsRadius", 1000.f);
	m_turnSpeed					= ParseXMLAttribute( element, "turnSpeed", 0.f );
	m_speed						= ParseXMLAttribute( element, "speed", 0.f );
}

// void EntityDefinition::InitializeEntityDefinitions( const XmlElement& rootEntityDefElement )
// {
// 
// }

