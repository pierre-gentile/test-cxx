#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <queue>

#include "thread.hpp"
#include "mutex.hpp"
#include "non-copyable.hpp"

namespace logger
{
	
	using namespace patterns;
	using namespace threading;
	
	class Level: private NonCopyable
	{
		public:
			inline Level(const string& name, unsigned int value):
					_name(name), _value(value)
			{
			}
		
			inline unsigned int value(void) const {
				return _value;
			}
			
			static const Level all;
			static const Level debug;
			static const Level info;
			static const Level warn;
			static const Level error;
			static const Level fatal;
			static const Level none;

		private:
			const string _name;
			unsigned int _value;
		
	};
	
	class Event: private NonCopyable
	{
		
		public:
			
			enum Kind {
				LOG_EVENT,
				SHUTDOWN	
			};
			
			Event(Kind kind);
			
			inline Kind kind(void) const {
				return _kind;
			}
		
		private:
			Kind _kind;
		
	};
	
	class LogEvent: public Event
	{
		public:
			LogEvent(const Level& level, const string& message);
			
			inline const Level& level(void) const {
				return _level;
			}
		
			inline const string& message(void) const {
				return _message;
			}

		private:
			const Level& _level;
			string _message;
	};
	
	class ShutdownEvent: public Event
	{
		public:
			ShutdownEvent(void);

	};

	class Consumer: public Thread
	{
		public:
			Consumer(queue<Event*>& pendingEvents, Mutex& mutex, Condition& publishedCond);
		
			virtual void run();
		
		private:
			queue<Event*>& _pendingEvents;
			Mutex& _mutex;
			Condition& _publishedCond;
	};

	class LoggerManager: private NonCopyable
	{
		public:
			LoggerManager(void);
			virtual ~LoggerManager();

			void log(const Level& level, const string& message);

		protected:
			void _publishEvent(Event* event);

		private:
			queue<Event*> _pendingEvents;
			Mutex _mutex;
			Condition _publishedCond;

			Consumer _consumer;
			
	};
	
}

#endif
