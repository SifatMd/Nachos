
#include "producer.h"
#include "system.h"
#include "stdlib.h"


Producer::Producer(const char* debugName, Condition *P, Condition *C, Lock *lock, List<int> *list, int s){
	name = debugName;
	this->P = P;
	this->C = C;
	this->lock = lock;
	elements = list;
	maxSize = s;
	
}


const char* Producer::getName(){
	return name;		
}

void Producer::produce(){
	int i, j, k=2, l=0;

	while(1){
		lock->Acquire(); //getting lock of table to put sth
		if(elements->getSize()>=maxSize){
			printf("List is full, producer going to sleep\n");
			P->Wait(lock);
		}
		else{
			int item = rand()%20;
			elements->Append(item);
			for(i=0;i<1000000000;i++);
			printf("%s produced %d, listSize: %d\n", this->getName(), item, elements->getSize());
	
		}

		lock->Release();
		C->Broadcast(lock);
		l++;
		if(l==k){
			currentThread->Yield();
			l=0;
			k = (rand()%5)+2;
		}
	}

}




