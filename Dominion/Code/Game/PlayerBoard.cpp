#include "Game/PlayerBoard.hpp"
#include "Game/CardDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Game.hpp"

PlayerBoard::PlayerBoard( RandomNumberGenerator* rand )
{
	m_rand = rand;
	m_hand.reserve( 10 );
	m_discardPile.reserve( 10 );
	m_playArea.reserve( 10 );
	m_deck.reserve( 30 );
}

PlayerBoard::PlayerBoard()
{
	m_hand.reserve( 10 );
	m_discardPile.reserve( 10 );
	m_playArea.reserve( 10 );
	m_deck.reserve( 30 );
}

void PlayerBoard::InitializeDeck( std::vector<CardDefinition const*>& deck )
{
	m_deck.swap( deck );
}

void PlayerBoard::ResetBoard()
{
	DiscardHand();
	DiscardPlayArea();
	AddDiscardPileToDeck();
	ShuffleDeck();
	Draw5();
}

void PlayerBoard::InitializeRand( RandomNumberGenerator* rand )
{
	m_rand = rand;
}

void PlayerBoard::AddCardToDiscardPile( CardDefinition const* cardToAdd )
{
	m_discardPile.push_back( cardToAdd );
}

std::vector<CardDefinition const*> const& PlayerBoard::GetHand() const
{
	return m_hand;
}

std::vector<CardDefinition const*>& PlayerBoard::GetPlayArea()
{
	return m_playArea;
}

CardDefinition const* PlayerBoard::TakeCardFromHand( size_t cardIndex )
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

int PlayerBoard::GetCurrentVictoryPoints() const
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

void PlayerBoard::ShuffleDeck()
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

void PlayerBoard::AddDiscardPileToDeck()
{
	m_deck.insert( m_deck.end(), m_discardPile.begin(), m_discardPile.end() );
	m_discardPile.clear();
}

void PlayerBoard::DiscardHand()
{
	m_discardPile.insert( m_discardPile.end(), m_hand.begin(), m_hand.end() );
	m_hand.clear();
}

void PlayerBoard::DiscardPlayArea()
{
	m_discardPile.insert( m_discardPile.end(), m_playArea.begin(), m_playArea.end() );
	m_playArea.clear();
}

void PlayerBoard::PlayTreasureCards()
{
	size_t handIndex = 0;
	while( handIndex < m_hand.size() )
	{
		CardDefinition const* card = m_hand[handIndex];
		if( nullptr == card )
		{
			ERROR_AND_DIE( "Card returned null." );
		}

		if( card->GetCardType() == TREASURE_TYPE )
		{
			m_currentCoins += card->GetCoins();
			PlayCard( handIndex, nullptr );
		}
		else
		{
			handIndex++;
		}
	}
}

void PlayerBoard::Draw( int numberToDraw )
{
	for( int drawIndex = 0; drawIndex < numberToDraw; drawIndex++ )
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


}

void PlayerBoard::Draw5()
{
	//Think about optimizing this;
	Draw( 5 );

}

void PlayerBoard::PlayCard( size_t handIndex, gamestate_t* gameState  )
{
	CardDefinition const* cardToPlay = TakeCardFromHand( handIndex );
	m_playArea.push_back( cardToPlay );

	if( cardToPlay->GetCardType() == ACTION_TYPE )
	{
		if( gameState )
		{
			int actionsToGain = cardToPlay->m_actionsGained;
			int cardDraw = cardToPlay->m_cardDraw;
			int buysToGain = cardToPlay->m_buysGained;
			int coinsToGain = cardToPlay->m_coins;

			m_numberOfActionsAvailable += actionsToGain;
			m_numberOfBuysAvailable += buysToGain;
			m_currentCoins += coinsToGain;
			Draw( cardDraw );

			if( cardToPlay->m_OpponentsDrawCard )
			{
				PlayerBoard* player1Deck = &gameState->m_playerBoards[0];
				PlayerBoard* player2Deck = &gameState->m_playerBoards[1];

				if( player1Deck != this )
				{
					player1Deck->Draw();
				}
				if( player2Deck != this )
				{
					player2Deck->Draw();
				}
			}

			if( cardToPlay->m_OpponentsGetCurse )
			{
				PlayerBoard* player1Deck = &gameState->m_playerBoards[0];
				PlayerBoard* player2Deck = &gameState->m_playerBoards[1];

				if( player1Deck != this )
				{
					pileData_t& pileData = gameState->m_cardPiles[(int)CURSE];
					if( pileData.m_pileSize > 0 )
					{
						pileData.m_pileSize -= 1;
						player1Deck->AddCardToDiscardPile( pileData.m_card );
					}
				}
				if( player2Deck != this )
				{
					pileData_t& pileData = gameState->m_cardPiles[(int)CURSE];
					if( pileData.m_pileSize > 0 )
					{
						pileData.m_pileSize -= 1;
						player2Deck->AddCardToDiscardPile( pileData.m_card );
					}
				}
			}
		}
		else
		{
			ERROR_AND_DIE("Gamestate is required for a action card to be played");
		}
	}

}

bool PlayerBoard::UnorderedCompare( PlayerBoard const& compare ) const
{
	bool isHandEqual = CardDefinition::UnorderedCompare( m_hand, compare.m_hand );
	bool isDiscardEqual = CardDefinition::UnorderedCompare( m_discardPile, compare.m_discardPile );
	bool isPlayAreaEqual = CardDefinition::UnorderedCompare( m_playArea, compare.m_playArea );
	bool isDeckEqual = CardDefinition::UnorderedCompare( m_deck, compare.m_deck );
	bool isCurrentCoinsEqual = ( m_currentCoins == compare.m_currentCoins );
	bool isActionsEqual = ( m_numberOfActionsAvailable == compare.m_numberOfActionsAvailable );
	bool isBuysEqual = ( m_numberOfBuysAvailable == compare.m_numberOfBuysAvailable );

	if( isHandEqual && isDiscardEqual && 
		isPlayAreaEqual && isCurrentCoinsEqual && 
		isActionsEqual && isBuysEqual && isDeckEqual )
	{
		return true;
	}

	return false;
}

