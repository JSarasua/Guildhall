#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include <string>
#include <vector>

typedef NamedStrings EventArgs;
typedef bool(*EventCallbackFunctionPtrType)(const EventArgs* args);


struct EventSubscription
{
public:
	std::string m_eventName;
	EventCallbackFunctionPtrType m_callbackfunctionPtr = nullptr;
};

class EventSystem
{
public:
	void SubscribeToEvent( const std::string& eventName, EventCallbackFunctionPtrType functionToCall);
	void UnsubscribeToEvent(const std::string& eventName);
	void FireEvent(const std::string& stringToCall, const EventArgs* args );

	static void ListEventSubscriptions( const EventArgs* args );



public:
	std::vector<EventSubscription*> m_eventSubscriptions;
};