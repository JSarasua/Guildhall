#include "Game/Deck.hpp"

Deck::Deck( RandomNumberGenerator* rand )
{
	m_rand = rand;
	m_hand.reserve( 10 );
	m_discardPile.reserve( 10 );
	m_playArea.reserve( 10 );
	m_deck.reserve( 30 );
}

void Deck::InitializeDeck( std::vector<int>& deck )
{
	m_deck.swap( deck );
}

void Deck::AddCardToDiscardPile( int cardToAdd )
{
	m_discardPile.push_back( cardToAdd );
}

std::vector<int>& Deck::GetHand()
{
	return m_hand;
}

int Deck::TakeCardFromHand( size_t cardIndex )
{
	int cardToTake = -1;
	if( !m_hand.empty() )
	{
		cardToTake = m_hand[cardIndex]; //Save off the card
		m_hand[cardIndex] = m_hand.back(); //Move the back element to where the taken card was
		m_hand.pop_back(); //Remove the last element
	}

	return cardToTake;
}

void Deck::ShuffleDeck()
{
	if( m_deck.empty() )
	{
		return;
	}

	int deckSize = (int)m_deck.size();
	for( int deckIndex = 0; deckIndex < deckSize; deckIndex++ )
	{
		int indexToSwapWith = m_rand->RollRandomIntInRange( deckIndex, deckSize - 1 );
		int firstCard = m_deck[deckIndex];
		int secondCard = m_deck[indexToSwapWith];

		m_deck[deckIndex] = secondCard;
		m_deck[indexToSwapWith] = firstCard;
	}
}

void Deck::AddDiscardPileToDeck()
{
	m_deck.insert( m_deck.end(), m_discardPile.begin(), m_discardPile.end() );
}

void Deck::DiscardHand()
{
	m_discardPile.insert( m_discardPile.end(), m_hand.begin(), m_hand.end() );
	m_hand.clear();
}

void Deck::DiscardPlayArea()
{
	m_discardPile.insert( m_discardPile.end(), m_playArea.begin(), m_hand.end() );
	m_playArea.clear();
}

void Deck::Draw()
{
	if( m_deck.empty() )
	{
		AddDiscardPileToDeck();
		ShuffleDeck();
	}

	if( !m_deck.empty() )
	{
		int cardToDraw = m_deck.back();
		m_deck.pop_back();
		m_hand.push_back( cardToDraw );
	}

}

void Deck::Draw5()
{
	//Think about optimizing this;
	Draw();
	Draw();
	Draw();
	Draw();
	Draw();
}

