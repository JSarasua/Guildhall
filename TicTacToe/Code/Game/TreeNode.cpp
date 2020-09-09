#include "Game/TreeNode.hpp"
#include "Game/Game.hpp"


TreeNode::~TreeNode()
{
	delete m_data;

	m_data = nullptr;
	m_parentNode = nullptr;

	for( size_t childIndex = 0; childIndex < m_childNodes.size(); childIndex++ )
	{
		delete m_childNodes[childIndex];
		m_childNodes[childIndex] = nullptr;
	}
}

