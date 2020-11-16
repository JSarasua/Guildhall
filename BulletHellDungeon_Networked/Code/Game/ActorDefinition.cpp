#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::map< std::string, ActorDefinition*> ActorDefinition::s_definitions;

int ActorDefinition::GetActorDefIndex( ActorDefinition const* actorDef )
{
	int index = 0;
	for( auto actorDefIter : s_definitions )
	{
		if( actorDefIter.second == actorDef )
		{
			break;
		}
		index++;
	}

	return index;
}

ActorDefinition const* ActorDefinition::GetActorDefFromIndex( int index )
{
	ActorDefinition const* actorDef = nullptr;
	int currentIndex = 0;
	for( auto actorDefIter : s_definitions )
	{
		if( currentIndex == index )
		{
			actorDef = actorDefIter.second;
			break;
		}
		currentIndex++;
	}

	return actorDef;
}

ActorDefinition::ActorDefinition( const XmlElement& element ) : EntityDefinition(element)
{
	IntVec2 spriteCoords		= ParseXMLAttribute( element, "spriteCoords", IntVec2( 0, 0 ) );
	m_tint						= ParseXMLAttribute( element, "spriteTint", Rgba8( 255, 255, 255 ) );
	m_drawBounds.mins			= ParseXMLAttribute( element, "minDrawBounds", Vec2( 0.f, 0.f ) );
	m_drawBounds.maxs			= ParseXMLAttribute( element, "maxDrawBounds", Vec2( 0.f, 0.f ) );
	IntVec2 portraitCoords		= ParseXMLAttribute( element, "spritePortraitCoords", IntVec2( 0, 0 ) );
	m_canWalk					= ParseXMLAttribute( element, "canWalk", false );
	m_canFly					= ParseXMLAttribute( element, "canFly", false );
	m_canSwim					= ParseXMLAttribute( element, "canSwim", false );
	m_waitingTime				= ParseXMLAttribute( element, "waitingTime", FloatRange(3.f,5.f));


	for( const XmlElement* childElement = element.FirstChildElement(); childElement; childElement = childElement->NextSiblingElement() )
	{
		const std::string name(childElement->Name());
		if( name == "SpriteAnimSet" )
		{
			m_actorAnimations = new SpriteAnimSet(*childElement, *g_actorSpriteSheet);
			break;
		}
	}



	int spriteIndex = g_actorSpriteSheet->GetSpriteIndex( spriteCoords );
	g_actorSpriteSheet->GetSpriteUVs( m_spriteUVs.mins, m_spriteUVs.maxs, spriteIndex );

	int portraitIndex = g_portraitSpriteSheet->GetSpriteIndex( portraitCoords );
	g_portraitSpriteSheet->GetSpriteUVs( m_spritePortraitUVs.mins, m_spritePortraitUVs.maxs, portraitIndex );
}

void ActorDefinition::InitializeActorDefinitions( const XmlElement& rootActorDefElement )
{
	for( const XmlElement* element = rootActorDefElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string actorDefName = nameAttribute->Value();
			ActorDefinition* actorDef = new ActorDefinition( *element );
			s_definitions[actorDefName] = actorDef;
		}
	}
}
