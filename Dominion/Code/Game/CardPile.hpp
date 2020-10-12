#pragma once
#include "Game/CardDefinition.hpp"
#include "Game/GameCommon.hpp"
#include <array>



class CardPile
{
public:
	CardPile() = default;
	~CardPile() = default;

	void Count( int index );
	void InsertPile( CardPile const& pileToAdd );
	void Clear();
private:
	std::array<pileData_t,NUMBEROFPILES> m_cards;
};