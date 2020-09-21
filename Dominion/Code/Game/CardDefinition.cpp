#include "Game/CardDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Core/StringUtils.hpp"

std::map< int, CardDefinition* > CardDefinition::s_definitions;

CardDefinition::CardDefinition( int cardIndex, int cost, int type, std::string cardName, int vp, int plusActions, int plusDraw ) :
	m_cardIndex( cardIndex ), 
	m_cost( cost ),
	m_type( type ),
	m_cardName( cardName ),
	m_actionsGained( plusActions ),
	m_cardDraw( plusDraw ),
	m_victoryPointValue( vp )
{}

//STATIC
CardDefinition const* CardDefinition::GetCardDefinitionByIndex( int cardIndex )
{
	auto cardIter = s_definitions.find( cardIndex );
	if( cardIter != s_definitions.end() )
	{
		return cardIter->second;
	}
	else
	{
		g_theConsole->ErrorString( Stringf( "Eror: Could not find card type with index: %i", cardIndex ) );
		return nullptr;
	}
}

