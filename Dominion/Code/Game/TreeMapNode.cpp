#include "Game/TreeMapNode.hpp"
#include "Engine/Core/BufferParser.hpp"
#include <stack>

constexpr byte startChildInputs = '[';
constexpr byte endChildInputs = ']';
constexpr byte startChildNodes = '(';
constexpr byte endChildNodes = ')';
constexpr byte inputSeparator = ',';
constexpr byte nodeSeparator = ';';

TreeMapNode::~TreeMapNode()
{
// 	delete m_data;
// 	m_data = nullptr;
	m_parentNode = nullptr;
	m_possibleOutcomes.clear();

	//auto childIter = m_possibleOutcomes.begin();

// 	for( auto childIter : m_possibleOutcomes )
// 	{
// 		std::vector<TreeMapNode*>& outcomes = childIter.second;
// 		for( size_t outcomeIndex = 0; outcomeIndex < outcomes.size(); outcomeIndex++ )
// 		{
// 			delete outcomes[outcomeIndex];
// 		}
// 	}
}

void TreeMapNode::DeleteTree( TreeMapNode* headNode )
{
	std::map<inputMove_t, std::vector<TreeMapNode*>>& moveMap = headNode->m_possibleOutcomes;
	std::map<inputMove_t, std::vector<TreeMapNode*>>::iterator nodeIter = moveMap.begin();
	int firstIndex = 0;

	TreeMapNodeDeleteData firstNodeData;
	firstNodeData.m_currentNode = headNode;
	firstNodeData.m_currentIterator = nodeIter;
	firstNodeData.m_currentOutcomeIndex = firstIndex;
	std::stack<TreeMapNodeDeleteData> deleteStack;
	
	deleteStack.push( firstNodeData );

	while( !deleteStack.empty() )
	{
		TreeMapNodeDeleteData& currentNodeData = deleteStack.top();
		TreeMapNode* currentNode = currentNodeData.m_currentNode;
		std::map<inputMove_t, std::vector<TreeMapNode*>>& currentMap = currentNode->m_possibleOutcomes;
		auto currentIterator = currentNodeData.m_currentIterator;
		if( currentIterator != currentMap.end() )
		{
			std::vector<TreeMapNode*>& currentOutcomes = currentIterator->second;
			int& currentIndex = currentNodeData.m_currentOutcomeIndex;
			if( currentIndex < currentOutcomes.size() )
			{
				TreeMapNode* nextNode = currentOutcomes[currentIndex];
				TreeMapNodeDeleteData nextNodeData;
				nextNodeData.m_currentNode = nextNode;
				nextNodeData.m_currentIterator = nextNode->m_possibleOutcomes.begin();
				deleteStack.push( nextNodeData );
				
				if( currentIndex + 1 < currentOutcomes.size() )
				{
					currentIndex++;
				}
				else
				{
					currentNodeData.m_currentIterator++;
				}
			}
			else
			{
				currentNodeData.m_currentIterator++;
			}
		}
		else
		{
			delete currentNode;
			deleteStack.pop();
		}
	}

	//deleteStack.push( TreeMapNodeDeleteData() );
	//if( nodeIter != headN )

	//loop forever
		//start with first iterator
		//start with first index
		//if exists push back current node and data + 1
			//if index < size index++
			//else iterator++
			//currentNode = that outcome
			//Restart loop
		//else
			//delete node
			//current node = parent node
		


}


void TreeMapNode::AppendTreeToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
{
	//Map Data
	m_data.AppendDataToBuffer( buffer, startIndex );
	AppendStartInput( buffer, startIndex ); // [
	
	for( auto outcomeIter : m_possibleOutcomes )
	{
		inputMove_t const& inputMove = outcomeIter.first;
		inputMove.AppendInputToBuffer( buffer, startIndex );

		AppendStartInputChildren( buffer, startIndex ); // (

		std::vector<TreeMapNode*> const& outcomes = outcomeIter.second;

		for( auto outcomeNode : outcomes )
		{
			outcomeNode->AppendTreeToBuffer( buffer, startIndex );

			//AppendNodeSeparator( buffer, startIndex ); // ,
		}

		AppendEndInputChildren( buffer, startIndex ); // )

		//AppendInputSeparator( buffer, startIndex ); // ;
	}

	AppendEndInput( buffer, startIndex ); // ]
}

void TreeMapNode::AppendStartInput( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &startChildInputs, sizeof( startChildInputs ), buffer, startIndex );
}

void TreeMapNode::AppendEndInput( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &endChildInputs, sizeof( endChildInputs ), buffer, startIndex );
}

void TreeMapNode::AppendStartInputChildren( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &startChildNodes, sizeof( startChildNodes ), buffer, startIndex );
}

void TreeMapNode::AppendEndInputChildren( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &endChildNodes, sizeof( endChildNodes ), buffer, startIndex );
}

void TreeMapNode::AppendInputSeparator( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &inputSeparator, sizeof( inputSeparator ), buffer, startIndex );
}

void TreeMapNode::AppendNodeSeparator( std::vector<byte>& buffer, size_t& startIndex ) const
{
	AppendDataToBuffer( &nodeSeparator, sizeof( nodeSeparator ), buffer, startIndex );
}

void TreeMapNode::AppendTreeToBufferWriter( BufferWriter& bufferWriter )
{
	//Map Data
	m_data.AppendDataToBufferWriter( bufferWriter );

	size_t childInputCount = m_possibleOutcomes.size();
	bufferWriter.AppendUShort( (uint16_t)childInputCount );

	for( auto outcomeIter : m_possibleOutcomes )
	{
		inputMove_t const& inputMove = outcomeIter.first;
		inputMove.AppendInputToBufferWriter( bufferWriter );

		std::vector<TreeMapNode*> const& outcomes = outcomeIter.second;
		size_t childNodeCount = outcomes.size();
		bufferWriter.AppendUShort( (uint16_t)childNodeCount );

		for( auto outcomeNode : outcomes )
		{
			outcomeNode->AppendTreeToBufferWriter( bufferWriter );
		}
	}
}

TreeMapNode* TreeMapNode::ParseDataFromBuffer( byte*& buffer )
{
	static byte* bufferPointerCheck = 0;
	static byte* buffer90kCheck = 0;
	if( bufferPointerCheck == 0 )
	{
		bufferPointerCheck = buffer;
		buffer90kCheck = buffer + 91500;
	}
	if( buffer > buffer90kCheck )
	{
		ERROR_AND_DIE("stop");
	}


	TreeMapNode* newNode = new TreeMapNode();
	newNode->m_data = data_t::ParseDataFromBuffer( buffer );

	if( *buffer == startChildInputs )
	{
		buffer += sizeof( startChildInputs );
		
		while( true )
		{
			if( *buffer == endChildInputs )
			{
				buffer += sizeof( endChildInputs );
				break;
			}
// 			else if( *buffer == inputSeparator )
// 			{
// 				buffer += sizeof( inputSeparator );
// 			}
			else
			{
				inputMove_t inputMove = inputMove_t::ParseInputFromBuffer( buffer );
				newNode->m_possibleOutcomes[inputMove] = std::vector<TreeMapNode*>();
				std::vector<TreeMapNode*>& newVectorOfOutComes = newNode->m_possibleOutcomes[inputMove];

				if( *buffer == startChildNodes )
				{
					buffer += sizeof( startChildNodes );

					while( true )
					{
						if( *buffer == endChildNodes )
						{
							buffer += sizeof( endChildNodes );
							break;
						}
// 						else if( *buffer == nodeSeparator )
// 						{
// 							buffer += sizeof( nodeSeparator );
// 						}
						else
						{
							TreeMapNode* childNode = TreeMapNode::ParseDataFromBuffer( buffer );
							childNode->m_parentNode = newNode;
							newVectorOfOutComes.push_back( childNode );
						}
					}
				}

			}
		}
	}
	return newNode;
	//Parse data

	//For loop on children
		//ParseData on child
}

TreeMapNode* TreeMapNode::ParseDataFromBufferParser( BufferParser& buffer )
{
	TreeMapNode* newNode = new TreeMapNode();
	newNode->m_data = data_t::ParseDataFromBufferParser( buffer );

	uint16_t childInputCount = buffer.ParseUShort();
	uint16_t childInputIndex = 0;

	while( childInputIndex < childInputCount )
	{
		inputMove_t inputMove = inputMove_t::ParseInputFromBufferParser( buffer );
		newNode->m_possibleOutcomes[inputMove] = std::vector<TreeMapNode*>();
		std::vector<TreeMapNode*>& newVectorOfOutComes = newNode->m_possibleOutcomes[inputMove];

		uint16_t childNodeCount = buffer.ParseUShort();
		uint16_t childNodeIndex = 0;

		while( childNodeIndex < childNodeCount )
		{
			TreeMapNode* childNode = TreeMapNode::ParseDataFromBufferParser( buffer );
			childNode->m_parentNode = newNode;
			newVectorOfOutComes.push_back( childNode );

			childNodeIndex++;
		}

		childInputIndex++;
	}

	return newNode;
}

