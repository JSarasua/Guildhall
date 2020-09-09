#pragma once
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Delegate.hpp"
// #include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>

//typedef NamedStrings EventArgs;
typedef NamedProperties EventArgs;
typedef bool(*EventCallbackFunctionPtrType)(const EventArgs* args);

enum eEventType
{
	CONSOLECOMMAND,
	NOCONSOLECOMMAND
};

struct EventSubscription
{
public:
	std::string m_eventName;
	eEventType m_eventType;
	Delegate<EventArgs const&> m_delegate;
	EventCallbackFunctionPtrType m_callbackfunctionPtr = nullptr;
};

class EventSystem
{
public:
	void SubscribeToEvent( const std::string& eventName, eEventType eventType, EventCallbackFunctionPtrType functionToCall );
	void UnsubscribeToEvent(const std::string& eventName);

	template <typename OBJ_TYPE>
	void SubscribeMethodToEvent( const std::string& eventName, eEventType eventType, OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb) (EventArgs const& args) )
	{
		EventSubscription* newSubscription = new EventSubscription();
		newSubscription->m_eventName = eventName;
		newSubscription->m_eventType = eventType;
		newSubscription->m_delegate.SubscribeMethod( obj, mcb );

		m_eventSubscriptions.push_back( newSubscription );
	}

	template <typename OBJ_TYPE>
	void UnsubscribeObject( OBJ_TYPE* obj )
	{
		for( auto iter: m_eventSubscriptions )
		{
			iter->m_delegate.UnsubscribeObject( obj	);
		}
	}

	void FireEvent(const std::string& stringToCall, eEventType eventType, const EventArgs* args );


	std::string GetFirstEventStartingWith( const std::string& subString, eEventType eventType );
private:
	EventArgs GetEvents();


public:
	std::vector<EventSubscription*> m_eventSubscriptions;

};