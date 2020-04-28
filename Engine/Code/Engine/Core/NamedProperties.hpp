#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <string>
#include <map>

class TypedPropertyBase
{
public:
	virtual ~TypedPropertyBase() {}

	virtual std::string GetAsString() const = 0;
	virtual void const* GetUniqueID() const = 0;

	template <typename T>
	bool Is() const
	{
		return GetUniqueID() == TypedProperty<T>::UNIQUE_ID;
	}

};

template<typename VALUE_TYPE>
class TypedProperty : public TypedPropertyBase
{
public:
	virtual std::string GetAsString() const final { return ToString( m_value ); }
	virtual void const* GetUniqueID() const final { return UniqueID; }

public:
	VALUE_TYPE m_value;

public:
	static int s_uniqueIdentifierLocation;
	static void const*const UNIQUE_ID = &s_uniqueIdentifierLocation;
};

//TypedProperty<int>::UNIQUE_ID != TypedProperty<float>::UNIQUE_ID

class NamedProperties
{
public:
	~NamedProperties() 
	{
	NEED TO DELETE ALL PROPERTIES HERE
	}

	void PopulateFromXMLAttributes( XmlElement const& element );
	void PopulateFromKeyValuePairs( std::string const& str ); //key=value key1=othervalue

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
		TypedPropertyBase* base = nullptr;
		auto iter = m_keyValuePairs.find( keyName );
		if( iter != m_keyValuePairs.end() )
		{
			base = *iter;
		
			//works without RTTI enabled, but doesn't work if prop inherits from T
			if( base->GetUniqueID() == TypedProperty<T>::UNIQUE_ID )
			{
				TypedProperty<T>* prop = (TypedProperty<T>*)base;
				return prop->m_value;
			}
			else
			{
				std::string strValue = base->GetAsString();
				return GetValueFromString( stValue, defaultValue );
			}
		}
		else
		{
			return defaultValue;
		}

	}
	

private:
	std::map<std::string, TypedPropertyBase*> m_keyValuePairs;
};