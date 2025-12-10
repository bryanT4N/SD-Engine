#pragma once

#include "Engine/Core/NamedStrings.hpp"

#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
// Event arguments are currently just a typedef to NamedStrings (key=value string pairs).
// In later courses this will evolve to a richer "name properties" type.
typedef NamedStrings EventArgs;

//-----------------------------------------------------------------------------------------------
// C-style function pointer type for event callbacks.
// Any subscriber must match this signature.
typedef bool (*EventCallbackFunction)( EventArgs& args );

//-----------------------------------------------------------------------------------------------
struct EventSubscription
{
	EventCallbackFunction m_callback = nullptr;
};

typedef std::vector<EventSubscription> SubscriptionList;

//-----------------------------------------------------------------------------------------------
struct EventSystemConfig
{
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
class EventSystem
{
public:
	EventSystem( EventSystemConfig const& config );
	~EventSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr );
	void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr );

	// Returns how many subscribers were notified for this event name.
	int  FireEvent( std::string const& eventName, EventArgs& args );
	int  FireEvent( std::string const& eventName );

protected:
	EventSystemConfig                                     m_config;
	std::map< std::string, SubscriptionList >            m_subscriptionListsByEventName;
};

//-----------------------------------------------------------------------------------------------
// Standalone helper functions that forward to the global Engine's EventSystem (if it exists).

void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr );
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction functionPtr );
int  FireEvent( std::string const& eventName, EventArgs& args );
int  FireEvent( std::string const& eventName );


