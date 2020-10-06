#include "Game/TreeMapNode.hpp"

TreeMapNode::~TreeMapNode()
{
	delete m_data;
	m_data = nullptr;
	m_parentNode = nullptr;

	//auto childIter = m_possibleOutcomes.begin();

	for( auto childIter : m_possibleOutcomes )
	{
		std::vector<TreeMapNode*>& outcomes = childIter.second;
		for( size_t outcomeIndex = 0; outcomeIndex < outcomes.size(); outcomeIndex++ )
		{
			delete outcomes[outcomeIndex];
		}
	}
}

