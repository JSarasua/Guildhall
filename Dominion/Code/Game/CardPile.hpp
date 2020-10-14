#pragma once
#include "Game/CardDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include <array>


class CardPile
{
public:
	CardPile() = default;
	~CardPile() = default;

	bool operator==( CardPile const& compare );
	int& operator[]( size_t index )				{ return m_cards[index]; }
	int const& operator[]( size_t index ) const	{ return m_cards[index]; }

	bool IsEmpty() const;
	int Count( int index ) const;
	void InsertPile( CardPile const& pileToAdd );
	void Clear();

private:
	std::array<int,NUMBEROFPILES> m_cards;
};