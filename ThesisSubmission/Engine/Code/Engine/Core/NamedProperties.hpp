#pragma once
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <string>
#include <map>

template<typename VALUE_TYPE>
class TypedProperty;

class TypedPropertyBase
{
public:
	virtual ~TypedPropertyBase() {}

	virtual std::string GetAsString() const = 0;
	virtual void const* GetUniqueID() const = 0;

	template <typename T>
	bool Is() const
	{
		return GetUniqueID() == TypedProperty<T>::StaticUniqueID();
	}

};

template<typename VALUE_TYPE>
class TypedProperty : public TypedPropertyBase
{
public:
	virtual std::string GetAsString() const final { return ToString( m_value ); }
	virtual void const* GetUniqueID() const final { return StaticUniqueID(); }

public:
	VALUE_TYPE m_value;

public:
	static void const* const StaticUniqueID()
	{
		static int s_local = 0;
		return &s_local;
	}
};


class NamedProperties
{
public:
	~NamedProperties() 
	{
		for( auto iter : m_keyValuePairs ) {
			delete iter.second;
		}

		m_keyValuePairs.clear();
	}

	void PopulateFromXMLAttributes( XmlElement const& element );
	void PopulateFromKeyValuePairs( std::string const& str ); //key=value key1=othervalue
	bool IsNotEmpty() const;

	void SetValue( std::string const& keyName, std::string const& valueName );

	//base string version
	std::string GetValue( std::string const& keyName, std::string const& defaultValue ) const;

	//Templates for everything else
	template <typename T>
	void SetValue( std::string const& keyName, T const& value )
	{
		TypedPropertyBase* base = FindInMap( keyName );
		if( nullptr == base )
		{
			TypedProperty<T>* prop = new TypedProperty<T>();
			prop->m_value = value;
			m_keyValuePairs[keyName] = prop;
		}
		else
		{
			if( base->Is<T>() )
			{
				//It exists
				TypedProperty<T>* prop = (TypedProperty<T>*)base;
				prop->m_value = value;
			}
			else
			{
				//Not the same type
				delete base;

				TypedProperty<T>* prop = new TypedProperty<T>();
				prop->m_value = value;
				m_keyValuePairs[keyName] = prop;
			}
		}
	}

	template <typename T>
	T GetValue( std::string const& keyName, T const& defaultValue ) const
	{
		TypedPropertyBase* base = FindInMap(keyName);
		if( nullptr != base )
		{
			//works without RTTI enabled, but doesn't work if prop inherits from T
			if( base->Is<T>() )
			{
				TypedProperty<T>* prop = (TypedProperty<T>*)base;
				return prop->m_value;
			}
			else
			{
				std::string strValue = base->GetAsString();
				return GetValueFromString( strValue, defaultValue );
			}
		}
		else
		{
			return defaultValue;
		}

	}
	
private:
	TypedPropertyBase* FindInMap( std::string const& key ) const
	{
		auto iter = m_keyValuePairs.find( key );
		if( iter != m_keyValuePairs.end() )
		{
			return iter->second;
		}
		else
		{
			return nullptr;
		}
	}

private:
	std::map<std::string, TypedPropertyBase*> m_keyValuePairs;
};