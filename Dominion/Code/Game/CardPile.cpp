#include "Game/CardPile.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"

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

int CardPile::GetNumberOfUniqueSimpleActions() const
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

int CardPile::GetCardIndexFromCountIndex( int countIndex )
{
	int cardNumToHit = countIndex + 1;
	int currentCount = 0;

	size_t pileCount = m_cards.size();
	for( size_t pileIndex = 0; pileIndex < pileCount; pileIndex++ )
	{
		currentCount += CountOfCard( (int)pileIndex );
		if( currentCount >= cardNumToHit )
		{
			return (int)pileIndex;
		}
	}

	return -1;
}

std::vector<eCards> CardPile::ToVector() const
{
	std::vector<eCards> cards;

	for( size_t cardIndex = 0; cardIndex < m_cards.size(); cardIndex++ )
	{
		eCards card = (eCards)cardIndex;
		size_t cardCount = m_cards[cardIndex];

		for( cardCount; cardCount > 0; cardCount-- )
		{
			cards.push_back( card );
		}
	}

	return cards;
}

void CardPile::ApppendCardPileToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
{
	size_t cardPileSize = sizeof(int) * m_cards.size();
	AppendDataToBuffer( (byte*)&m_cards[0], cardPileSize, buffer, startIndex );
}

void CardPile::AppendCardPileToBufferWriter( BufferWriter& bufferWriter ) const
{
	for( int const& cardCount : m_cards )
	{
		bufferWriter.AppendInt32( cardCount );
	}
}

void CardPile::ParseFromBuffer( byte*& buffer )
{
	for( int& cardCount : m_cards )
	{
		cardCount = ParseInt( buffer );
	}
}

void CardPile::ParseFromBufferParser( BufferParser& bufferParser )
{
	for( int& cardCount : m_cards )
	{
		cardCount = bufferParser.ParseInt32();
	}
}

CardPile CardPile::ParseCardPileFromBuffer( byte*& buffer )
{
	CardPile cardPile;
	cardPile.ParseFromBuffer( buffer );

	return cardPile;
}

CardPile CardPile::ParseCardPileFromBufferParser( BufferParser& bufferParser )
{
	CardPile cardPile;
	cardPile.ParseFromBufferParser( bufferParser );

	return cardPile;
}

