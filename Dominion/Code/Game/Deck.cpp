#include "Game/Deck.hpp"
#include "Game/CardDefinition.hpp"

Deck::Deck( RandomNumberGenerator* rand )
{
	m_rand = rand;
	m_hand.reserve( 10 );
	m_discardPile.reserve( 10 );
	m_playArea.reserve( 10 );
	m_deck.reserve( 30 );
}

Deck::Deck()
{
	m_hand.reserve( 10 );
	m_discardPile.reserve( 10 );
	m_playArea.reserve( 10 );
	m_deck.reserve( 30 );
}

void Deck::InitializeDeck( std::vector<CardDefinition const*>& deck )
{
	m_deck.swap( deck );
}

void Deck::InitializeRand( RandomNumberGenerator* rand )
{
	m_rand = rand;
}

void Deck::AddCardToDiscardPile( CardDefinition const* cardToAdd )
{
	m_discardPile.push_back( cardToAdd );
}

std::vector<CardDefinition const*>& Deck::GetHand()
{
	return m_hand;
}

CardDefinition const* Deck::TakeCardFromHand( size_t cardIndex )
{
	CardDefinition const* cardToTake = nullptr;
	if( !m_hand.empty() )
	{
		cardToTake = m_hand[cardIndex]; //Save off the card
		m_hand[cardIndex] = m_hand.back(); //Move the back element to where the taken card was
		m_hand.pop_back(); //Remove the last element
	}

	return cardToTake;
}

int Deck::GetCurrentVictoryPoints() const
{
	int currentVPCount = 0;

	size_t handSize = m_hand.size();
	for( size_t handIndex = 0; handIndex < handSize; handIndex++ )
	{
		int cardVPs = m_hand[handIndex]->GetCardVPs();
		currentVPCount += cardVPs;
	}
	size_t discardSize = m_discardPile.size();
	for( size_t discardIndex = 0; discardIndex < discardSize; discardIndex++ )
	{
		int cardVPs = m_discardPile[discardIndex]->GetCardVPs();
		currentVPCount += cardVPs;
	}
	size_t playerAreaSize = m_playArea.size();
	for( size_t playerAreaIndex = 0; playerAreaIndex < playerAreaSize; playerAreaIndex++ )
	{
		int cardVPs = m_playArea[playerAreaIndex]->GetCardVPs();
		currentVPCount += cardVPs;
	}
	size_t deckSize = m_deck.size();
	for( size_t deckIndex = 0; deckIndex < deckSize; deckIndex++ )
	{
		int cardVPs = m_deck[deckIndex]->GetCardVPs();
		currentVPCount += cardVPs;
	}

	return currentVPCount;
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
		CardDefinition const* firstCard = m_deck[deckIndex];
		CardDefinition const* secondCard = m_deck[indexToSwapWith];

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
		CardDefinition const* cardToDraw = m_deck.back();
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

