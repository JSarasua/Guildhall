#include "Game/PlayerBoard.hpp"
//#include "Game/CardDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/FileUtils.hpp"

PlayerBoard::PlayerBoard()
{

	m_deck.reserve( 30 );
}

void PlayerBoard::InitializeDeck( std::vector<CardData_t>& deck )
{
	m_deck.swap( deck );

	for( size_t deckIndex = 0; deckIndex < m_deck.size(); deckIndex++ )
	{
		m_sortedDeck.AddCard( m_deck[deckIndex].cardIndex );
	}
}

void PlayerBoard::ResetBoard()
{
	DiscardHand();
	DiscardPlayArea();
	AddDiscardPileToDeck();
	ShuffleDeck();
	Draw5();
}

void PlayerBoard::AddCardToDiscardPile( int cardIndex )
{
	m_discardPile.AddCard( cardIndex );
}

CardPile const& PlayerBoard::GetHand() const
{
	return m_hand;
}

CardPile const& PlayerBoard::GetPlayArea() const
{
	return m_playArea;
}

void PlayerBoard::TakeCardFromHand( size_t cardIndex )
{
	m_hand.RemoveCard( (int)cardIndex );
}

int PlayerBoard::GetCurrentVictoryPoints() const
{


	int currentVPCount = 0;

	int countOfProvinces = 0;
	countOfProvinces += m_hand.CountOfCard( PROVINCE );
	countOfProvinces += m_discardPile.CountOfCard( PROVINCE );
	countOfProvinces += m_playArea.CountOfCard( PROVINCE );
	countOfProvinces += m_sortedDeck.CountOfCard( PROVINCE );
	currentVPCount += countOfProvinces * 6;

	int countOfDuchies = 0;
	countOfDuchies += m_hand.CountOfCard( DUCHY );
	countOfDuchies += m_discardPile.CountOfCard( DUCHY );
	countOfDuchies += m_playArea.CountOfCard( DUCHY );
	countOfDuchies += m_sortedDeck.CountOfCard( DUCHY );
	currentVPCount += countOfDuchies * 3;

	int countOfEstates = 0;
	countOfEstates += m_hand.CountOfCard( ESTATE );
	countOfEstates += m_discardPile.CountOfCard( ESTATE );
	countOfEstates += m_playArea.CountOfCard( ESTATE );
	countOfEstates += m_sortedDeck.CountOfCard( ESTATE );
	currentVPCount += countOfEstates; //Worth 1 VP

	int countOfCurses = 0;
	countOfCurses += m_hand.CountOfCard( CURSE );
	countOfCurses += m_discardPile.CountOfCard( CURSE );
	countOfCurses += m_playArea.CountOfCard( CURSE );
	countOfCurses += m_sortedDeck.CountOfCard( CURSE );
	currentVPCount -= countOfCurses; //Worth -1 VP

	return currentVPCount;
}

void PlayerBoard::ShuffleDeck()
{
	if( m_deck.empty() )
	{
		return;
	}
	
	RandomNumberGenerator& rng = g_theGame->m_rand;

	int deckSize = (int)m_deck.size();
	for( int deckIndex = 0; deckIndex < deckSize; deckIndex++ )
	{
		int indexToSwapWith = rng.RollRandomIntInRange( deckIndex, deckSize - 1 );
		CardData_t firstCard = m_deck[deckIndex];
		CardData_t secondCard = m_deck[indexToSwapWith];

		m_deck[deckIndex] = secondCard;
		m_deck[indexToSwapWith] = firstCard;
	}
}

void PlayerBoard::AddDiscardPileToDeck()
{
	size_t uniqueCardCount = m_discardPile.GetNumberOfPossibleUniqueCards();
	for( size_t cardIndex = 0; cardIndex < uniqueCardCount; cardIndex++ )
	{
		int pileSize = m_discardPile[cardIndex];
		if( pileSize > 0 )
		{
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
			for( int pileIndex = 0; pileIndex < pileSize; pileIndex++ )
			{
				m_deck.emplace_back( card, (int)cardIndex );
			}
		}
	}
	m_sortedDeck.InsertPile( m_discardPile );

	m_discardPile.Clear();
}

void PlayerBoard::DiscardHand()
{
	m_discardPile.InsertPile( m_hand );
	m_hand.Clear();
}

void PlayerBoard::DiscardPlayArea()
{
	m_discardPile.InsertPile( m_playArea );
	m_playArea.Clear();
}

void PlayerBoard::PlayTreasureCards()
{
	int copperCardCount = m_hand.CountOfCard( COPPER );
	int silverCardCount = m_hand.CountOfCard( SILVER );
	int goldCardCount = m_hand.CountOfCard( GOLD );

	m_currentCoins += copperCardCount;
	m_currentCoins += silverCardCount * 2;
	m_currentCoins += goldCardCount * 3;

	m_hand.RemoveCard( COPPER, copperCardCount );
	m_hand.RemoveCard( SILVER, silverCardCount );
	m_hand.RemoveCard( GOLD, goldCardCount );

	m_playArea.AddCard( COPPER, copperCardCount );
	m_playArea.AddCard( SILVER, silverCardCount );
	m_playArea.AddCard( GOLD, goldCardCount );
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
			CardData_t& cardData = m_deck.back();
			m_hand.AddCard( cardData.cardIndex );
			m_deck.pop_back();
			m_sortedDeck.RemoveCard( cardData.cardIndex );
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
	if( m_hand[handIndex] <= 0 )
	{
		return;
	}

	m_hand.RemoveCard( (int)handIndex );
	m_playArea.AddCard( (int)handIndex );
	CardDefinition const* cardToPlay = CardDefinition::GetCardDefinitionByType( (eCards)handIndex );

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
						player1Deck->m_discardPile.AddCard( CURSE );
					}
				}
				if( player2Deck != this )
				{
					pileData_t& pileData = gameState->m_cardPiles[(int)CURSE];
					if( pileData.m_pileSize > 0 )
					{
						pileData.m_pileSize -= 1;
						player2Deck->m_discardPile.AddCard( CURSE );
					}
				}
			}

			if( cardToPlay->m_discardUptoHandSizeToDrawThatMany )
			{

			}

			if( cardToPlay->m_trashUpToFourCards )
			{

			}
		}
		else
		{
			ERROR_AND_DIE("Gamestate is required for a action card to be played");
		}
	}

}

bool PlayerBoard::CanPlayCard( int handIndex, gamestate_t const* gameState ) const
{
	UNUSED( gameState );
	if( m_numberOfActionsAvailable > 0 )
	{
		if( m_hand.CountOfCard( handIndex ) > 0 )
		{
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)handIndex );
			if( card && card->GetCardType() == ACTION_TYPE )
			{
				return true;
			}
		}
	}

	return false;
}

int PlayerBoard::GetNumberOfValidActionsToPlay() const
{
	if( m_numberOfActionsAvailable > 0 )
	{
		return m_hand.GetNumberOfUniqueActions();
	}

	return 0;
}

bool PlayerBoard::UnorderedCompare( PlayerBoard const& compare ) const
{
	bool isHandEqual = m_hand == compare.m_hand;
	bool isDiscardEqual = m_discardPile == compare.m_discardPile;
	bool isPlayAreaEqual = m_playArea == compare.m_playArea;
	bool isDeckEqual = m_sortedDeck == compare.m_sortedDeck;
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

bool PlayerBoard::HasCard( int cardIndex ) const
{
	if( m_hand.CountOfCard( cardIndex ) ||
		m_discardPile.CountOfCard( cardIndex ) ||
		m_playArea.CountOfCard( cardIndex ) ||
		m_sortedDeck.CountOfCard( cardIndex ) )
	{
		return true;
	}

	return false;
}

int PlayerBoard::GetCardIndexFromHandIndex( int handIndex )
{
	return m_hand.GetCardIndexFromCountIndex( handIndex );
}

int PlayerBoard::GetCountOfCard( int cardIndex ) const
{
	int handCount = m_hand.CountOfCard( cardIndex );
	int playAreaCount = m_playArea.CountOfCard( cardIndex );
	int discardPileCount = m_discardPile.CountOfCard( cardIndex );
	int deckCount = m_sortedDeck.CountOfCard( cardIndex );

	int cardCount = handCount + playAreaCount + discardPileCount + deckCount;

	return cardCount;
}

void PlayerBoard::RandomizeHandAndDeck()
{
	int handCount = m_hand.TotalCount();
	AddHandToDeck();
	ShuffleDeck();
	Draw( handCount );
}

void PlayerBoard::AppendPlayerBoardToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
{
	m_hand.ApppendCardPileToBuffer( buffer, startIndex );
	m_discardPile.ApppendCardPileToBuffer( buffer, startIndex );
	m_playArea.ApppendCardPileToBuffer( buffer, startIndex );
	m_sortedDeck.ApppendCardPileToBuffer( buffer, startIndex );

	AppendDataToBuffer( (byte*)&m_currentCoins, sizeof( int ), buffer, startIndex );
	AppendDataToBuffer( (byte*)&m_numberOfActionsAvailable, sizeof( int ), buffer, startIndex );
	AppendDataToBuffer( (byte*)&m_numberOfBuysAvailable, sizeof( int ), buffer, startIndex );


	for( size_t deckIndex = 0; deckIndex < m_deck.size(); deckIndex++ )
	{
		m_deck[deckIndex].AppendCardDataToBuffer( buffer, startIndex );
	}

	AppendDataToBuffer( (byte*)&ENDDECKBYTES, 2, buffer, startIndex );
}

void PlayerBoard::ParseFromBuffer( byte*& buffer )
{
	m_hand.ParseFromBuffer( buffer );
	m_discardPile.ParseFromBuffer( buffer );
	m_playArea.ParseFromBuffer( buffer );
	m_sortedDeck.ParseFromBuffer( buffer );

	m_currentCoins = ParseInt( buffer );
	m_numberOfActionsAvailable = ParseInt( buffer );
	m_numberOfBuysAvailable = ParseInt( buffer );

	byte* endDeckCheck = buffer;
	int breakCounter = 0;
	int MaxCount = m_sortedDeck.TotalCount();

	while( memcmp( endDeckCheck, ENDDECKBYTES, CHECKSIZE ) != 0 )
	{
		breakCounter++;
		if( breakCounter > MaxCount )
		{
			ERROR_AND_DIE( "Attempting to Parse player board passed count of sorted deck")
		}
		CardData_t cardData;
		cardData.ParseCardDataFromBuffer( buffer );
		m_deck.push_back( cardData );

		endDeckCheck = buffer;
	}

	buffer += CHECKSIZE;
}

PlayerBoard PlayerBoard::ParsePlayerBoardFromBuffer( byte*& buffer )
{
	PlayerBoard playerBoard;
	playerBoard.ParseFromBuffer( buffer );
	return playerBoard;
}

void PlayerBoard::AddHandToDeck()
{
	size_t uniqueCardCount = m_hand.GetNumberOfPossibleUniqueCards();
	for( size_t cardIndex = 0; cardIndex < uniqueCardCount; cardIndex++ )
	{
		int pileSize = m_hand[cardIndex];
		if( pileSize > 0 )
		{
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
			for( int pileIndex = 0; pileIndex < pileSize; pileIndex++ )
			{
				m_deck.emplace_back( card, (int)cardIndex );
			}
		}
	}
	m_sortedDeck.InsertPile( m_hand );

	m_hand.Clear();
}
