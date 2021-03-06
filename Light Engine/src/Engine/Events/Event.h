#pragma once

#include "Core/Core.h"

#include <ostream>

#include <functional>

namespace Light {

	enum EventCategory
	{
		EventCategory_Input    = BIT(0),
		EventCategory_Mouse    = BIT(1),
		EventCategory_Keyboard = BIT(2),
		EventCategory_Window   = BIT(3),
	};

	enum class EventType
	{
		WindowResized, WindowMoved, WindowFocused, WindowLostFocus,
		WindowRestored, WindowMaximized, WindowMinimized, WindowClosed,

		MouseMoved, MouseButtonPressed, MouseButtonReleased, MouseScrolled,
		KeyboardKeyPressed, KeyboardKeyReleased,
		GamepadButtonPressed, GamepadButtonReleased, GamepadAxisMoved, GamepadAxisTriggered,
	};

	
#define EVENT_TYPE(type) EventType GetEventType  () const override { return EventType::##type; } \
                  static EventType GetStaticType ()                { return EventType::##type; } \

#define EVENT_CATEGORY(Category) inline bool IsInCategory(EventCategory category) const override { return (Category) & category; }


	class Event
	{
	private:
		friend class Dispatcher;
		bool b_Dispatched = false;
	public:
		Event() = default;

		// getters
		virtual inline EventType   GetEventType() const = 0;
		virtual        std::string GetLogInfo  () const = 0;

		virtual inline bool IsInCategory(EventCategory category) const = 0;

		inline bool IsDispatched() const { return b_Dispatched; }

		// operators
		friend inline std::ostream& operator<<(std::ostream& os, const Event& event) // why can't I use this on spdlog????
		{
			return os << event.GetLogInfo();
		}
	};

	class Dispatcher
	{
	private:
		Event& m_Event;
	public:
		Dispatcher(Event& event): m_Event(event) {}

		template <typename EventType>
		void Dispatch(std::function<bool(EventType&)> function)
		{
			if(m_Event.GetEventType() ==  EventType::GetStaticType())
				m_Event.b_Dispatched = function(static_cast<EventType&>(m_Event));
		}
	};

}