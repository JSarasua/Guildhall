#include "Game/CardPile.hpp"

bool CardPile::operator==( CardPile const& compare )
{
	if( m_cards == compare.m_cards )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CardPile::IsEmpty() const
{
	size_t numCards = m_cards.size();
	for( size_t cardIndex = 0; cardIndex < numCards; cardIndex++ )
	{
		int cardCount = m_cards[cardIndex];
		if( cardCount > 0 )
		{
			return false;
		}
	}

	return true;
}

int CardPile::Count( int index ) const
{
	return m_cards[index];
}

void CardPile::InsertPile( CardPile const& pileToAdd )
{
	size_t numCards = m_cards.size();
	for( size_t cardIndex = 0; cardIndex < numCards; cardIndex++ )
	{
		m_cards[cardIndex] += pileToAdd.m_cards[cardIndex];
	}
}

void CardPile::Clear()
{
	size_t numCards = m_cards.size();
	for( size_t cardIndex = 0; cardIndex < numCards; cardIndex++ )
	{
		m_cards[cardIndex] = 0;
	}
}

