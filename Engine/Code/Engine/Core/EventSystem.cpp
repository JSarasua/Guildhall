#include "Engine/Core/EventSystem.hpp"



void EventSystem::SubscribeToEvent( const std::string& eventName, EventCallbackFunctionPtrType functionToCall )
{
	EventSubscription* newSubscription = new EventSubscription();
	newSubscription->m_eventName = eventName;
	newSubscription->m_callbackfunctionPtr = functionToCall;

	m_eventSubscriptions.push_back(newSubscription);
}

void EventSystem::UnsubscribeToEvent( const std::string& eventName )
{
	for( int eventSystemIndex = 0; eventSystemIndex < m_eventSubscriptions.size(); eventSystemIndex++ )
	{
		if( !m_eventSubscriptions[eventSystemIndex] )
		{
			continue;
		}
		const std::string& name = m_eventSubscriptions[eventSystemIndex]->m_eventName;
		if( name == eventName )
		{
			delete m_eventSubscriptions[eventSystemIndex];
			m_eventSubscriptions[eventSystemIndex] = nullptr;
		}
	}
}

void EventSystem::FireEvent( const std::string& stringToCall, const EventArgs* args )
{
	for( int eventSystemIndex = 0; eventSystemIndex < m_eventSubscriptions.size(); eventSystemIndex++ )
	{
		if( !m_eventSubscriptions[eventSystemIndex] )
		{
			continue;
		}

		if( m_eventSubscriptions[eventSystemIndex]->m_eventName == stringToCall )
		{
			EventCallbackFunctionPtrType functionToCall = m_eventSubscriptions[eventSystemIndex]->m_callbackfunctionPtr;
			bool isEventUsed = functionToCall(args);
			
			if( isEventUsed )
			{
				return;
			}
		}
	}
}

