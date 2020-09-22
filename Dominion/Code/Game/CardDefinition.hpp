#pragma once
#include <string>
#include <map>

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

	NUM_CARDS

};

class CardDefinition
{
public:
	CardDefinition() = delete;
	CardDefinition( int cardIndex, int type, std::string cardName, int cost = 0, int coins = 0, int vp = 0, int plusActions = 0, int plusDraw = 0 );
	~CardDefinition() {}

	static void InitializeCards();
	static CardDefinition const* GetCardDefinitionByType( eCards cardType );
	static std::map< eCards, CardDefinition* > s_definitions;

	int GetCardCost() const { return m_cost; }
	int GetCardType() const { return m_type; }
	int GetCardVPs() const	{ return m_victoryPointValue; }
	std::string const& GetCardName() const { return m_cardName; }

public:
	int m_cardIndex = -1;
	int m_cost = -1;
	int m_type = -1;
	int m_coins = -1;
	std::string m_cardName;
	int m_victoryPointValue = 0;
	int m_actionsGained = 0;
	int m_cardDraw = 0;


};