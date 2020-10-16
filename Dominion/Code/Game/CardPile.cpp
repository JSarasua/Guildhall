#include "Game/CardPile.hpp"
#include "Game/Game.hpp"

bool CardPile::operator==( CardPile const& compare ) const
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

int CardPile::CountOfCard( int index ) const
{
	return m_cards[index];
}

int CardPile::TotalCount() const
{
	int totalCount = 0;

	for( int pileCount : m_cards )
	{
		totalCount += pileCount;
	}

	return totalCount;
}

void CardPile::InsertPile( CardPile const& pileToAdd )
{
	size_t numCards = m_cards.size();
	for( size_t cardIndex = 0; cardIndex < numCards; cardIndex++ )
	{
		m_cards[cardIndex] += pileToAdd.m_cards[cardIndex];
	}
}



void CardPile::AddCard( int cardIndex, int count /*= 1 */ )
{
	if( cardIndex >= 0 && cardIndex < m_cards.size() )
	{
		m_cards[cardIndex] += count;
	}
}

void CardPile::RemoveCard( int cardIndex, int count /*= 1*/ )
{
	if( cardIndex >= 0 && cardIndex < m_cards.size() )
	{
		m_cards[cardIndex] -= count;
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

int CardPile::GetNumberOfUniqueActions() const
{
	int uniqueActionCount = 0;
	uniqueActionCount += (bool)m_cards[Village];
	uniqueActionCount += (bool)m_cards[Smithy];
	uniqueActionCount += (bool)m_cards[CouncilRoom];
	uniqueActionCount += (bool)m_cards[Festival];
	uniqueActionCount += (bool)m_cards[Laboratory];
	uniqueActionCount += (bool)m_cards[Market];
	uniqueActionCount += (bool)m_cards[Witch];

	return uniqueActionCount;
}

