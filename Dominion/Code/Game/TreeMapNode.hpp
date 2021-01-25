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

	void AppendTreeToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendStartInput( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendEndInput( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendStartInputChildren( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendEndInputChildren( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendInputSeparator( std::vector<byte>& buffer, size_t& startIndex ) const;
	void AppendNodeSeparator( std::vector<byte>& buffer, size_t& startIndex ) const;

	static TreeMapNode* ParseDataFromBuffer( byte*& buffer );

	void AppendNodeToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const;
	static TreeMapNode* ParseNodeFromBuffer( byte*& buffer );
public:
	//Don't do pointer
	//data_t* m_data = nullptr;
	data_t m_data;

	TreeMapNode* m_parentNode = nullptr;

	//Change to a map of input to vectors of childnodes
	//std::vector<TreeMapNode*> m_childNodes;

	std::map< inputMove_t, std::vector<TreeMapNode*> > m_possibleOutcomes;
};
