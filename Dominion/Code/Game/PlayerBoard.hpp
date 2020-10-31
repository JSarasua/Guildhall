#pragma once
#include <deque>
#include <vector>
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/CardPile.hpp"
#include "Game/CardDefinition.hpp"

//class CardDefinition;

struct gamestate_t;

struct CardData_t
{
public:
	CardData_t() = default;
	~CardData_t() = default;
	CardData_t( CardDefinition const* cardToAdd, int cardIndexToAdd ) :
		card( cardToAdd ), 
		cardIndex( cardIndexToAdd ) 
	{}

	bool operator<( CardData_t const& compare ) const
	{
		if( card < compare.card )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	CardDefinition const* card = nullptr;
	int cardIndex = -1;
};

class PlayerBoard
{
public:
	PlayerBoard();
	PlayerBoard( RandomNumberGenerator* rand);
	~PlayerBoard() = default;

	void InitializeDeck( std::vector<CardData_t>& deck );
	void ResetBoard();
	void InitializeRand( RandomNumberGenerator* rand );
	void AddCardToDiscardPile( int cardIndex );

	CardPile const& GetHand() const;
	CardPile const& GetPlayArea() const;
	void TakeCardFromHand( size_t cardIndex );

	int GetCurrentMoney() const { return m_currentCoins; }
	int GetCurrentVictoryPoints() const;
	void DecrementCoins( int cost ) { m_currentCoins -= cost; }
	void ResetCurrentCoins() { m_currentCoins = 0; }
	void ShuffleDeck();
	void AddDiscardPileToDeck();
	void DiscardHand();
	void DiscardPlayArea();
	void PlayTreasureCards();
	void Draw( int numberToDraw = 1 );
	void Draw5();
	void PlayCard( size_t handIndex, gamestate_t* gameState );
	bool CanPlayCard( int handIndex, gamestate_t const* gameState ) const;

	size_t GetDeckSize() { return m_deck.size(); }
	size_t GetDiscardSize() { return (size_t)m_discardPile.TotalCount(); }
	int GetNumberOfValidActionsToPlay() const;
	bool UnorderedCompare( PlayerBoard const& compare ) const;
	bool HasCard( int cardIndex ) const;
	int GetCardIndexFromHandIndex( int handIndex );
	int GetCountOfCard( int cardIndex ) const;

public:

	CardPile m_hand;
	CardPile m_discardPile;
	CardPile m_playArea;
	CardPile m_sortedDeck;
	int m_currentCoins = 0;
	int m_numberOfActionsAvailable = 1;
	int m_numberOfBuysAvailable = 1;
private:
	std::vector<CardData_t> m_deck;
	RandomNumberGenerator* m_rand = nullptr;

};