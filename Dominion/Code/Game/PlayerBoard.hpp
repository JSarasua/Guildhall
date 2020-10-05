#pragma once
#include <deque>
#include <vector>
#include "Engine/Math/RandomNumberGenerator.hpp"

class CardDefinition;

struct gamestate_t;

class PlayerBoard
{
public:
	PlayerBoard();
	PlayerBoard( RandomNumberGenerator* rand);
	~PlayerBoard() = default;

	void InitializeDeck( std::vector<CardDefinition const*>& deck );
	void ResetBoard();
	void InitializeRand( RandomNumberGenerator* rand );
	void AddCardToDiscardPile( CardDefinition const* cardToAdd );

	std::vector<CardDefinition const*> const& GetHand() const;
	std::vector<CardDefinition const*>& GetPlayArea();
	CardDefinition const* TakeCardFromHand( size_t cardIndex );

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

	size_t GetDeckSize() { return m_deck.size(); }
	size_t GetDiscardSize() { return m_discardPile.size(); }

	bool UnorderedCompare( PlayerBoard const& compare ) const;

public:
	std::vector<CardDefinition const*> m_hand;
	std::vector<CardDefinition const*> m_discardPile;
	std::vector<CardDefinition const*> m_playArea;
	int m_currentCoins = 0;
	int m_numberOfActionsAvailable = 1;
	int m_numberOfBuysAvailable = 1;
private:
	std::vector<CardDefinition const*> m_deck;
	RandomNumberGenerator* m_rand = nullptr;

};