
#include "synch.h"


class Producer{
	private:
		const char* name;
		Condition* P;
		Condition* C;
		Lock *lock;
		List<int> *elements;
		int maxSize;
	
	public:
		Producer(const char* debugName, Condition *P, Condition *C, Lock *lock, List<int> *list, int s);
		const char* getName();
		void produce();
};
