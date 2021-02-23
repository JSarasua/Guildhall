#pragma once
#include "Game/GameCommon.hpp"
#include <array>
#include "Engine/Core/EngineCommon.hpp"

class BufferParser;
class BufferWriter;

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
	int GetNumberOfUniqueSimpleActions() const; //Does not include Remodel
	int GetCardIndexFromCountIndex( int countIndex ); //The count index is if you had a hand of cards, what is the third card

	void ApppendCardPileToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendCardPileToBufferWriter( BufferWriter& bufferWriter ) const;

	void ParseFromBuffer( byte*& buffer );
	void ParseFromBufferParser( BufferParser& bufferParser );

	static CardPile ParseCardPileFromBuffer( byte*& buffer );
	static CardPile ParseCardPileFromBufferParser( BufferParser& bufferParser );

private:
	static const int m_numberOfPossibleUniqueCards = NUMBEROFPILES;
	
	std::array<int,m_numberOfPossibleUniqueCards> m_cards;
};