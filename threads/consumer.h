
#include "synch.h"



class Consumer{
	private:
		const char* name;
		Condition* P;
		Condition* C;
		Lock *lock;
		List<int> *elements;
		int maxSize;
	
	public:
		Consumer(const char* debugName, Condition *P, Condition *C, Lock *lock, List<int> *list, int s);
		const char* getName();
		void consume();
};
