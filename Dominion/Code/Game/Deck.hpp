#pragma once
#include <deque>
#include <vector>
#include "Engine/Math/RandomNumberGenerator.hpp"

class CardDefinition;

class Deck
{
public:
	Deck();
	Deck( RandomNumberGenerator* rand);
	~Deck() = default;

	void InitializeDeck( std::vector<CardDefinition const*>& deck );
	void InitializeRand( RandomNumberGenerator* rand );
	void AddCardToDiscardPile( CardDefinition const* cardToAdd );

	std::vector<CardDefinition const*>& GetHand();
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
	void Draw();
	void Draw5();
	void PlayCard( size_t handIndex );

	size_t GetDeckSize() { return m_deck.size(); }
	size_t GetDiscardSize() { return m_discardPile.size(); }

public:
	std::vector<CardDefinition const*> m_hand;
	std::vector<CardDefinition const*> m_discardPile;
	std::vector<CardDefinition const*> m_playArea;
	int m_currentCoins = 0;
private:
	std::vector<CardDefinition const*> m_deck;
	RandomNumberGenerator* m_rand = nullptr;

};