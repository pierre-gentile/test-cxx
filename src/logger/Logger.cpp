#include "logger/Logger.hpp"

#include <pthread.h>

#include <limits>
#include <memory>
#include <iostream>

namespace logger
{

	using namespace std;
	using namespace logger;
	
	// Class Level
	
	Level::Level(const string& name, unsigned int value):
			_name(name), _value(value)
	{
	}
	
	bool Level::operator<(const Level& other) const
	{
		return _value < other._value;
	}
	
	bool Level::operator==(const Level& other) const
	{
		return _value == other._value;
	}
	
	ostream& operator<<(ostream& out, const Level& level)
	{
		out << level._name;
		return out;
	}
	
	const Level Level::all("ALL", numeric_limits<unsigned int>::min());
	const Level Level::debug("DEBUG", 500);
	const Level Level::info("INFO", 1000);
	const Level Level::warn("WARN", 1500);
	const Level Level::error("ERROR", 2000);
	const Level Level::fatal("FATAL", 2500);
	const Level Level::none("NONE", numeric_limits<unsigned int>::max());
	
	// Class Consumer

	void Consumer::run()
	{
		//cout << "Demarrage consommateur" << endl;
		
		bool running = true;
		
		while (running) {
			_extractedEvents.clear();
			_queue.extract(_extractedEvents);
			
			for (vector<Event*>::iterator it = _extractedEvents.begin(); it != _extractedEvents.end(); ++it) {
				auto_ptr<Event> event(*it); // Supprimer l'evenement en sortie de scope
				Event::Kind kind = event->kind();
				if (kind == Event::LOG_EVENT) {
					LogEvent* logEvent = static_cast<LogEvent*>(event.get());
					cout << "LOGGER EVENT: (" << logEvent->level() << ") " << logEvent->message() << endl;
				} else if (kind == Event::SHUTDOWN) {
					running = false;
				}
			}
		}
		
		//cout << "Arret consommateur" << endl;
	}
	
	Consumer::Consumer(EventQueue& queue): _queue(queue)
	{
	}
	
	// Class Event
	
	Event::Event(Kind kind): _kind(kind)
	{
	}
	
	// Class EventQueue
	
	EventQueue::EventQueue(unsigned int size):
			_size(size), _count(0),
			_events(new Event*[size]),
			_publishedCond(_mutex)
	{
	}
	
	bool EventQueue::publish(Event* event)
	{
		bool published = false;
		MutexLock lock(_mutex);
		
		//cout << "Before publish: count=" << _count << ", size=" << _size << endl;
		
		// Ajouter le nouvel element, si possible
		if (_count < _size) {
			//cout << "Published event at position " << _count << endl;
			_events[_count] = event;
			_count++;
			published = true;
		} else {
			Event* last = _events[_size - 1];
			if (*event > *last) {
				cerr << "Event queue full, dropping old event, replacing with new event" << endl;
				delete last; // Drop old
				_events[_size - 1] = event; // Append new
			} else {
				cerr << "Event queue full, dropping new event" << endl;
				delete event; // Drop new
			}
		}
		
		_publishedCond.signal();
		
		//cout << "After publish: count=" << _count << ", size=" << _size << endl << endl;
		
		return published;
	}
	
	void EventQueue::extract(vector<Event*>& output)
	{
		MutexLock lock(_mutex);
		
		if (_count > 0) {
			// Copier les pointeurs dans output
			for (unsigned int i = 0; i < _count; i++) {
				output.push_back(_events[i]);
			}
			_count = 0;
		} else {
			// Attendre l'arrivee d'un evenement
			_publishedCond.wait();
		}
	}
	
	EventQueue::~EventQueue()
	{
		// Supprimer les evenements restants
		for (unsigned int i = 0; i < _count; i++) {
			cout << "Dropping event at position " << i << endl;
			delete _events[i];
		}
		
		delete[] _events;
	}
	
	// Class LogEvent
	
	LogEvent::LogEvent(const Level& level, const string& message):
			Event(LOG_EVENT), _level(level), _message(message)
	{
	}

	bool LogEvent::operator<(const Event& other) const
	{
		bool result = false;
		Kind kind = other.kind();
		if (kind == Event::SHUTDOWN) {
			result = true;
		} else { // kind == Event::LOG_EVENT
			const LogEvent* otherLogEvent = static_cast<const LogEvent*>(&other);
			return _level < otherLogEvent->_level;
		}
		return result;
	}
	
	bool LogEvent::operator==(const Event& other) const
	{
		bool result = false;
		if (this->kind() == other.kind()) {
			const LogEvent& otherLogEvent = static_cast<const LogEvent&>(other);
			result = _level == otherLogEvent._level;
		}
		return result;
	}
	
	// Class ShutdownEvent
	
	ShutdownEvent::ShutdownEvent(void): Event(SHUTDOWN)
	{
	}
	
	bool ShutdownEvent::operator==(const Event& other) const
	{
		return this->kind() == other.kind();
	}
	
	bool ShutdownEvent::operator<(const Event& other) const
	{
		return false;
	}
	
	// Class LoggerManager

	LoggerManager::LoggerManager(const Level& threshold, unsigned int queueSize):
			_threshold(threshold),
			_queue(queueSize),
	 		_consumer(_queue)
	{
		_consumer.start();
	}

	LoggerManager::~LoggerManager()
	{
		Event* event = new ShutdownEvent();
		_queue.publish(event);
		
		_consumer.join();
	}
	
	void LoggerManager::log(const Level& level, const string& message)
	{
		if (level >= _threshold) {
			Event* event = new LogEvent(level, message);
			_queue.publish(event);
		}
	}

}
