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
	CardDefinition const* TakeCardFromHand( size_t cardIndex );

	int GetCurrentVictoryPoints() const;

	void ShuffleDeck();
	void AddDiscardPileToDeck();
	void DiscardHand();
	void DiscardPlayArea();
	void Draw();
	void Draw5();

public:
	std::vector<CardDefinition const*> m_hand;
	std::vector<CardDefinition const*> m_discardPile;
	std::vector<CardDefinition const*> m_playArea;
private:
	std::vector<CardDefinition const*> m_deck;
	RandomNumberGenerator* m_rand = nullptr;

};