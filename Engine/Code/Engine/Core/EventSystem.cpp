#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
EventSystem::EventSystem( EventSystemConfig const& config )
	: m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
EventSystem::~EventSystem()
{
}

//-----------------------------------------------------------------------------------------------
void EventSystem::Startup()
{
	// Nothing yet; kept for symmetry and future expansion.
}

//-----------------------------------------------------------------------------------------------
void EventSystem::Shutdown()
{
	m_subscriptionListsByEventName.clear();
}

//-----------------------------------------------------------------------------------------------
void EventSystem::BeginFrame()
{
	// Reserved for future per-frame work (e.g. queued events).
}

//-----------------------------------------------------------------------------------------------
void EventSystem::EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr )
{
	if( eventName.empty() )
	{
		return;
	}
	if( functionPtr == nullptr )
	{
		return;
	}

	SubscriptionList& list = m_subscriptionListsByEventName[eventName];

	EventSubscription subscription;
	subscription.m_callback = functionPtr;
	list.push_back( subscription );
}

//-----------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr )
{
	if( functionPtr == nullptr )
	{
		return;
	}

	std::map< std::string, SubscriptionList >::iterator found = m_subscriptionListsByEventName.find( eventName );
	if( found == m_subscriptionListsByEventName.end() )
	{
		return;
	}

	SubscriptionList& list = found->second;
	for( size_t index = 0; index < list.size(); ++index )
	{
		if( list[index].m_callback == functionPtr )
		{
			list.erase( list.begin() + static_cast<long>( index ) );
			--index;
		}
	}
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	int numRecipients = 0;

	std::map< std::string, SubscriptionList >::iterator found = m_subscriptionListsByEventName.find( eventName );
	if( found == m_subscriptionListsByEventName.end() )
	{
		return 0;
	}

	SubscriptionList& list = found->second;
	for( size_t index = 0; index < list.size(); ++index )
	{
		EventCallbackFunction callback = list[index].m_callback;
		if( callback == nullptr )
		{
			continue;
		}

		bool consumed = callback( args );
		++numRecipients;

		if( consumed )
		{
			break;
		}
	}

	return numRecipients;
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs emptyArgs;
	return FireEvent( eventName, emptyArgs );
}

//-----------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr )
{
	if( g_engine == nullptr )
	{
		return;
	}
	if( g_engine->m_eventSystem == nullptr )
	{
		return;
	}

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( eventName, functionPtr );
}

//-----------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr )
{
	if( g_engine == nullptr )
	{
		return;
	}
	if( g_engine->m_eventSystem == nullptr )
	{
		return;
	}

	g_engine->m_eventSystem->UnsubscribeEventCallbackFunction( eventName, functionPtr );
}

//-----------------------------------------------------------------------------------------------
int FireEvent( std::string const& eventName, EventArgs& args )
{
	if( g_engine == nullptr )
	{
		return 0;
	}
	if( g_engine->m_eventSystem == nullptr )
	{
		return 0;
	}

	return g_engine->m_eventSystem->FireEvent( eventName, args );
}

//-----------------------------------------------------------------------------------------------
int FireEvent( std::string const& eventName )
{
	if( g_engine == nullptr )
	{
		return 0;
	}
	if( g_engine->m_eventSystem == nullptr )
	{
		return 0;
	}

	return g_engine->m_eventSystem->FireEvent( eventName );
}


