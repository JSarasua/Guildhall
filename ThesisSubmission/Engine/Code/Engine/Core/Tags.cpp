#include "Engine/Core/Tags.hpp"

Tags::Tags( std::vector<std::string> strings )
{
	m_tags = strings;
}

void Tags::SetTags( std::vector<std::string>& tags )
{
	m_tags = tags;
}

void Tags::AddTag( const std::string& tag )
{
	m_tags.push_back(tag);
}

void Tags::AppendTags( const Tags& tags )
{
	for( int tagsIndex = 0; tagsIndex < tags.m_tags.size(); tagsIndex++ )
	{
		m_tags.push_back(tags.m_tags[tagsIndex]);
	}
}

void Tags::RemoveTag( const std::string& tag )
{
	for( int tagIndex = 0; tagIndex < m_tags.size(); tagIndex++ )
	{
		if( m_tags[tagIndex] == tag )
		{
			m_tags[tagIndex].erase();
			break;
		}
	}
}

void Tags::ClearAllTags()
{
	m_tags.clear();
}

bool Tags::HasTag( const std::string& tag )
{
	for( int tagIndex = 0; tagIndex < m_tags.size(); tagIndex++ )
	{
		if( m_tags[tagIndex] == tag )
		{
			return true;
		}
	}
	return false;
}

bool Tags::HasTags( const std::vector<std::string>& tags )
{
	for( int tagsToCheckIndex = 0; tagsToCheckIndex < tags.size(); tagsToCheckIndex++ )
	{
		for( int tagIndex = 0; tagIndex < m_tags.size(); tagIndex++ )
		{
			if( tags[tagsToCheckIndex] == m_tags[tagIndex] )
			{
				continue;
			}
			return false;
		}
	}

	return true;
}

bool Tags::HasTags( const Tags& tags )
{
	std::vector<bool> hasTags;
	for( int hasTagsIndex = 0; hasTagsIndex < tags.m_tags.size(); hasTagsIndex++ )
	{
		hasTags.push_back(false);
	}


	for( int tagsToCheckIndex = 0; tagsToCheckIndex < tags.m_tags.size(); tagsToCheckIndex++ )
	{
		for( int tagIndex = 0; tagIndex < m_tags.size(); tagIndex++ )
		{
			if( tags.m_tags[tagsToCheckIndex] == m_tags[tagIndex] )
			{
				hasTags[tagsToCheckIndex] = true;
				continue;
			}
			return false;
		}
		if( !hasTags[tagsToCheckIndex] )
		{
			return false;
		}
	}


	return true;
}

