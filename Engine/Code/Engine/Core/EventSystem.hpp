#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include <string>
#include <vector>

typedef NamedStrings EventArgs;
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
	EventCallbackFunctionPtrType m_callbackfunctionPtr = nullptr;
};

class EventSystem
{
public:
	void SubscribeToEvent( const std::string& eventName, eEventType eventType, EventCallbackFunctionPtrType functionToCall );
	void UnsubscribeToEvent(const std::string& eventName);
	void FireEvent(const std::string& stringToCall, eEventType eventType, const EventArgs* args );


	std::string GetFirstEventStartingWith( const std::string& subString, eEventType eventType );
private:
	EventArgs GetEvents();


public:
	std::vector<EventSubscription*> m_eventSubscriptions;

};