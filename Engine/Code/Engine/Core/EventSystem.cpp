#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <algorithm>
#include <cctype>

//-----------------------------------------------------------------------------------------------
static char ToLowerCharacter(unsigned char c)
{
	return static_cast<char>(std::tolower(c));
}

//-----------------------------------------------------------------------------------------------
static std::string NormalizeEventName(std::string const& eventName)
{
	std::string normalized = eventName;
	std::transform(normalized.begin(), normalized.end(), normalized.begin(),
		ToLowerCharacter);
	return normalized;
}

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

	std::string normalizedEventName = NormalizeEventName(eventName);
	SubscriptionList& list = m_subscriptionListsByEventName[normalizedEventName];

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

	std::string normalizedEventName = NormalizeEventName(eventName);
	std::map< std::string, SubscriptionList >::iterator found = m_subscriptionListsByEventName.find( normalizedEventName );
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
Strings EventSystem::GetRegisteredCommandNames() const
{
	Strings names;
	names.reserve(m_subscriptionListsByEventName.size());

	for (std::map< std::string, SubscriptionList >::const_iterator iter = m_subscriptionListsByEventName.begin();
		iter != m_subscriptionListsByEventName.end();
		++iter)
	{
		names.push_back(iter->first);
	}

	return names;
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	int numRecipients = 0;

	std::string normalizedEventName = NormalizeEventName(eventName);
	std::map< std::string, SubscriptionList >::iterator found = m_subscriptionListsByEventName.find( normalizedEventName );
	if( found == m_subscriptionListsByEventName.end() )
	{
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(
				DevConsole::LOG_COLOR_ERROR,
				Stringf("Unknown command: %s", eventName.c_str()));
		}
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
Strings GetRegisteredCommandNames()
{
	if (g_engine == nullptr || g_engine->m_eventSystem == nullptr) {
		return Strings();
	}

	return g_engine->m_eventSystem->GetRegisteredCommandNames();
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


