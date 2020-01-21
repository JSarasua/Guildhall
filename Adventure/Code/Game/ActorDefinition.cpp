#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::map< std::string, ActorDefinition*> ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition( const XMLElement& element )
{
	m_name						= ParseXMLAttribute( element, "name", "INVALID");
	IntVec2 spriteCoords		= ParseXMLAttribute( element, "spriteCoords", IntVec2( 0, 0 ) );
	m_tint						= ParseXMLAttribute( element, "spriteTint", Rgba8( 255, 255, 255 ) );
	m_drawBounds.mins			= ParseXMLAttribute( element, "minDrawBounds", Vec2( 0.f, 0.f ) );
	m_drawBounds.maxs			= ParseXMLAttribute( element, "maxDrawBounds", Vec2( 0.f, 0.f ) );
	m_physicsRadius				= ParseXMLAttribute( element, "physicsRadius", 1000.f);
	m_turnSpeed					= ParseXMLAttribute( element, "turnSpeed", 0.f );
	m_speed						= ParseXMLAttribute( element, "speed", 0.f );
	m_canWalk					= ParseXMLAttribute( element, "canWalk", false );
	m_canFly					= ParseXMLAttribute( element, "canFly", false );
	m_canSwim					= ParseXMLAttribute( element, "canSwim", false );




	int spriteIndex = g_actorSpriteSheet->GetSpriteIndex( spriteCoords );
	g_actorSpriteSheet->GetSpriteUVs( m_spriteUVs.mins, m_spriteUVs.maxs, spriteIndex );
}

void ActorDefinition::InitializeActorDefinitions( const XMLElement& rootActorDefElement )
{
	for( const XMLElement* element = rootActorDefElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XMLAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string actorDefName = nameAttribute->Value();
			ActorDefinition* actorDef = new ActorDefinition( *element );
			s_definitions[actorDefName] = actorDef;
		}
	}
}
