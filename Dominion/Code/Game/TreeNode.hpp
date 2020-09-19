#pragma once
#include <vector>

struct data_t;

class TreeNode
{
public:
TreeNode() = default;
~TreeNode();


public:
	data_t* m_data = nullptr;

	TreeNode* m_parentNode = nullptr;
	std::vector<TreeNode*> m_childNodes;
};
