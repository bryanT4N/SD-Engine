#pragma once

//-----------------------------------------------------------------------------------------------
class UIWidget;

//-----------------------------------------------------------------------------------------------
struct UIEventReply
{
	bool		handled			= false;
	UIWidget*	requestCapture	= nullptr;
	bool		releaseCapture	= false;
	UIWidget*	requestFocus	= nullptr;
	bool		releaseFocus	= false;

	static UIEventReply Handled()
	{
		UIEventReply reply;
		reply.handled = true;
		return reply;
	}

	static UIEventReply Unhandled()
	{
		return {};
	}

	UIEventReply& CaptureMouse(UIWidget* widget)
	{
		requestCapture = widget;
		return *this;
	}

	UIEventReply& ReleaseMouseCapture()
	{
		releaseCapture = true;
		return *this;
	}

	UIEventReply& SetFocus(UIWidget* widget)
	{
		requestFocus = widget;
		return *this;
	}

	UIEventReply& ReleaseFocus()
	{
		releaseFocus = true;
		return *this;
	}
};
