#pragma once
#include <string>
#include <map>

enum eCardType
{
	InvalidType = -1,
	Action,
	Treasure,
	Victory,

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

	//Action

	NUM_CARDS

};

class CardDefinition
{
public:
	CardDefinition() = delete;
	CardDefinition( int cardIndex, int cost, int type, std::string cardName, int vp = 0, int plusActions = 0, int plusDraw = 0 );
	~CardDefinition() {}

	static CardDefinition const* GetCardDefinitionByIndex( int cardIndex );
	static std::map< int, CardDefinition* > s_definitions;

	int GetCardCost() const { return m_cost; }
	int GetCardType() const { return m_type; }
	std::string const& GetCardName() const { return m_cardName; }

public:
	int m_cardIndex = -1;
	int m_cost = -1;
	int m_type = -1;
	std::string m_cardName;
	int m_victoryPointValue = 0;
	int m_actionsGained = 0;
	int m_cardDraw = 0;


};