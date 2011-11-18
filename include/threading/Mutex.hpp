#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

#include <iostream>
#include <stdexcept>

#include "patterns/NonCopyable.hpp"

namespace threading {
	
	using namespace patterns;
	using namespace std;
	
	class MutexLock;
	class Condition;
	
	/**
	 * Mutex
	 *
	 * <p>Protecting a critical section:</p>
	 *
	 * <pre>
	 * Mutex mutex;
	 * {
	 *    	MutexLock lock(mutex);
	 *      // Critical section here
	 * }
	 * </pre>
	 *
	 * @see MutexLock
	 */
	class Mutex: private NonCopyable
	{
		
		public:
			
			Mutex(void);
		
			void lock(void);
		
			bool tryLock(void);
		
			void release(void);
		
			virtual ~Mutex(void);
		
		private:
			
			void _checkRC(int result);
		
			pthread_mutex_t _mutex;
		
			friend class MutexLock;
			friend class Condition;
		
	};
	
	/**
	 * Lock on a mutex
	 */
	class MutexLock: private NonCopyable
	{
		
		public:
			
			MutexLock(Mutex& mutex);
		
			virtual ~MutexLock();

		private:
			Mutex& _mutex;
		
	};
	
	class Condition: private NonCopyable
	{
	
		public:
			
			Condition(Mutex& mutex);
		
			void wait(void);
		
			void signal(void);
		
			void broadcast(void);
		
			virtual ~Condition();
		
			inline Mutex& mutex(void) {
				return _mutex;
			}
		
		private:
		
			Mutex& _mutex;
			pthread_cond_t _cond;
		
	};
	
}

#endif
