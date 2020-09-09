#pragma once
#include <string>
#include <vector>


struct Tags
{
public:
	std::vector<std::string> m_tags;
	
public:
	Tags() = default;
	Tags(std::vector<std::string> strings);

	//Mutators
	void SetTags( std::vector<std::string>& tags );
	void AddTag( const std::string& tag );
	void AppendTags( const Tags& tags );
	void RemoveTag( const std::string& tag );
	void ClearAllTags();
	
	//Checks
	bool HasTag( const std::string& tag );
	bool HasTags( const std::vector<std::string>& tags );
	bool HasTags( const Tags& tags );
};