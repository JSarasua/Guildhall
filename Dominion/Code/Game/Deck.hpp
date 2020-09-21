#pragma once
#include <deque>
#include <vector>
#include "Engine/Math/RandomNumberGenerator.hpp"

class Deck
{
public:
	Deck( RandomNumberGenerator* rand);
	~Deck() = default;

	void InitializeDeck( std::vector<int>& deck );
	void AddCardToDiscardPile( int cardToAdd );

	std::vector<int>& GetHand();
	int TakeCardFromHand( size_t cardIndex );

	void ShuffleDeck();
	void AddDiscardPileToDeck();
	void DiscardHand();
	void DiscardPlayArea();
	void Draw();
	void Draw5();

public:
	std::vector<int> m_hand;
	std::vector<int> m_discardPile;
	std::vector<int> m_playArea;
private:
	std::vector<int> m_deck;
	RandomNumberGenerator* m_rand = nullptr;

};