#pragma once
#include "Game/GameCommon.hpp"
#include <array>


class CardPile
{
public:
	CardPile() = default;
	~CardPile() = default;

	bool operator==( CardPile const& compare ) const;
	int& operator[]( size_t index )				{ return m_cards[index]; }
	int const& operator[]( size_t index ) const	{ return m_cards[index]; }

	static int GetNumberOfPossibleUniqueCards() { return m_numberOfPossibleUniqueCards; }
	
	bool IsEmpty() const;
	int CountOfCard( int index ) const;
	int TotalCount() const;
	void InsertPile( CardPile const& pileToAdd );
	void AddCard( int cardIndex, int count = 1 );
	void RemoveCard( int cardIndex, int count = 1);
	void Clear();
	int GetNumberOfUniqueActions() const;

private:
	static const int m_numberOfPossibleUniqueCards = NUMBEROFPILES;
	
	std::array<int,m_numberOfPossibleUniqueCards> m_cards;
};