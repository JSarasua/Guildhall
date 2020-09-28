#include "Game/CardDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Core/StringUtils.hpp"

std::map< eCards, CardDefinition* > CardDefinition::s_definitions;

CardDefinition::CardDefinition( int cardIndex, eCardType type, std::string cardName, int cost, int coins, int vp, int plusActions, int plusDraw, int plusBuys ) :
	m_cardIndex( cardIndex ), 
	m_cost( cost ),
	m_coins( coins ),
	m_type( type ),
	m_cardName( cardName ),
	m_actionsGained( plusActions ),
	m_cardDraw( plusDraw ),
	m_victoryPointValue( vp ),
	m_buysGained( plusBuys )
{}

void CardDefinition::InitializeCards()
{																													//Cost		Coins		VP		Actions	Draw	Buy
	CardDefinition* copperCard		= new CardDefinition( eCards::COPPER,	eCardType::TREASURE_TYPE,	"Copper",		 0,			1,		 0,		 0,		0,		0 );
	CardDefinition* silverCard		= new CardDefinition( eCards::SILVER,	eCardType::TREASURE_TYPE,	"Silver",		 3,			2,		 0,		 0,		0,		0 );
	CardDefinition* goldCard		= new CardDefinition( eCards::GOLD,		eCardType::TREASURE_TYPE,	"Gold",			 6,			3,		 0,		 0,		0,		0 );
	CardDefinition* estateCard		= new CardDefinition( eCards::ESTATE,	eCardType::VICTORY_TYPE,	"Estate",		 2,			0,		 1,		 0,		0,		0 );
	CardDefinition* duchyCard		= new CardDefinition( eCards::DUCHY,	eCardType::VICTORY_TYPE,	"Duchy",		 5,			0,		 3,		 0,		0,		0 );
	CardDefinition* provinceCard	= new CardDefinition( eCards::PROVINCE, eCardType::VICTORY_TYPE,	"Province",		 8,			0,		 6,		 0,		0,		0 );
	CardDefinition* curseCard		= new CardDefinition( eCards::CURSE,	eCardType::CURSE_TYPE,		"Curse",		 0,			0,		 -1,	 0,		0,		0 );

	CardDefinition* villageCard		= new CardDefinition( eCards::Village,		eCardType::ACTION_TYPE, "Village",		3,			0,		 0,		 2,		 1,		0 );
	CardDefinition* smithyCard		= new CardDefinition( eCards::Smithy,		eCardType::ACTION_TYPE, "Smithy",		4,			0,		 0,		 0,		 3,		0 );
	//CardDefinition* councilRoomCard	= new CardDefinition( eCards::CouncilRoom,	eCardType::ACTION_TYPE, "Council Room", 5, 0, -1, 0, 0 );
	CardDefinition* festivalCard	= new CardDefinition( eCards::Festival,		eCardType::ACTION_TYPE, "Festival",		5,			2,		 0,		 2,		 0,		1 );
	CardDefinition* laboraryCard	= new CardDefinition( eCards::Laboratory,	eCardType::ACTION_TYPE, "Laboratory",	5,			0,		 0,		 1,		 2,		0 );
	CardDefinition* marketCard		= new CardDefinition( eCards::Market,		eCardType::ACTION_TYPE, "Market",		5,			1,		 0,		 1,		 1,		1 );
	//CardDefinition* witchCard		= new CardDefinition( eCards::Witch,		eCardType::ACTION_TYPE, "Witch",		5,			0,		 0,		 0,		 2,		0 );

// 	Village,
// 		Smithy,
// 		CouncilRoom,
// 		Festival,
// 		Laboratory,
// 		Market,
// 		Witch,

	s_definitions[eCards::COPPER]		= copperCard;
	s_definitions[eCards::SILVER]		= silverCard;
	s_definitions[eCards::GOLD]			= goldCard;
	s_definitions[eCards::ESTATE]		= estateCard;
	s_definitions[eCards::DUCHY]		= duchyCard;
	s_definitions[eCards::PROVINCE]		= provinceCard;
	s_definitions[eCards::CURSE]		= curseCard;
	s_definitions[eCards::Village]		= villageCard;
	s_definitions[eCards::Smithy]		= smithyCard;
	s_definitions[eCards::Festival]		= festivalCard;
	s_definitions[eCards::Laboratory]	= laboraryCard;
	s_definitions[eCards::Market]		= marketCard;
}

//STATIC
CardDefinition const* CardDefinition::GetCardDefinitionByType( eCards cardType )
{
	auto cardIter = s_definitions.find( cardType );
	if( cardIter != s_definitions.end() )
	{
		return cardIter->second;
	}
	else
	{
		g_theConsole->ErrorString( Stringf( "Eror: Could not find card type with index: %i", (int)cardType ) );
		return nullptr;
	}
}

