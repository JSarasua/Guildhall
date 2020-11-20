#pragma once
#include <string>
#include <map>
#include <vector>

enum eCardType
{
	InvalidType = -1,
	ACTION_TYPE,
	CURSE_TYPE,
	TREASURE_TYPE,
	VICTORY_TYPE,

	NumCardTypes
};

enum eCards
{
	INVALID_CARD = -1,

	//Treasure
	COPPER,
	SILVER,
	GOLD,

	//Victory
	ESTATE,
	DUCHY,
	PROVINCE,

	//Curse
	CURSE,
	
	//Action
	Village, //Make sure Village is always first action, code depends on it
	Smithy,
	CouncilRoom,
	Festival,
	Laboratory,
	Market,
	Witch,

	NUM_CARDS
};

class CardDefinition
{
public:
	CardDefinition() = delete;
	CardDefinition( int cardIndex, eCardType type, std::string cardName, int cost = 0, int coins = 0, int vp = 0, int plusActions = 0, int plusDraw = 0, int plusBuys = 0 );
	~CardDefinition() {}

	static void InitializeCards();
	static CardDefinition const* GetCardDefinitionByType( eCards cardType );
	//static std::map< eCards, CardDefinition* > s_definitions;
	static std::vector<CardDefinition*> s_definitions;

	static bool UnorderedCompare( std::vector<CardDefinition const*> const& first, std::vector<CardDefinition const*> const& second );

	int GetCardCost() const { return m_cost; }
	int GetCoins() const { return m_coins; }
	eCardType GetCardType() const { return m_type; }
	int GetCardVPs() const	{ return m_victoryPointValue; }
	std::string const& GetCardName() const { return m_cardName; }

public:
	int m_cardIndex = -1;
	int m_cost = -1;
	eCardType m_type = InvalidType;
	int m_coins = -1;
	std::string m_cardName;
	int m_victoryPointValue = 0;
	int m_actionsGained = 0;
	int m_cardDraw = 0;
	int m_buysGained = 0;


	bool m_OpponentsGetCurse = false;
	bool m_OpponentsDrawCard = false;
	bool m_trashUpToFourCards = false;
	bool m_discardUptoHandSizeToDrawThatMany = false;

};