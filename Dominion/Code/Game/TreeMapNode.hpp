#pragma once
#include <vector>
#include <map>
#include "Game/Game.hpp"

struct data_t;

class TreeMapNode
{
public:
	TreeMapNode() = default;
	~TreeMapNode();


public:
	//Don't do pointer
	data_t* m_data = nullptr;

	TreeMapNode* m_parentNode = nullptr;

	//Change to a map of input to vectors of childnodes
	//std::vector<TreeMapNode*> m_childNodes;

	std::map< inputMove_t, std::vector<TreeMapNode*> > m_possibleOutcomes;
};
