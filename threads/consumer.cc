#include "system.h"
#include "consumer.h"
#include "stdlib.h"


Consumer::Consumer(const char* debugName, Condition *P, Condition *C, Lock *lock, List<int> *list, int s){
	name = debugName;
	this->P = P;
	this->C = C;
	this->lock = lock;
	elements = list;
	maxSize = s;
}


const char* Consumer::getName(){
	return name;		
}


void Consumer::consume(){
	int i, j, k=1, l=0;

	while(1){
		lock->Acquire(); //getting lock of table to get sth
		if(elements->IsEmpty()){
			printf("List is empty, consumer going to sleep\n");
			C->Wait(lock);
		}
		else{
			int item = elements->Remove();
			for(i=0;i<1000000000;i++);
			printf("%s consumed %d, listSize: %d\n", this->getName(), item, elements->getSize());
 
		}

		lock->Release();
		P->Broadcast(lock);
		l++;
		if(l==k){
			currentThread->Yield();
			l=0;
			k = (rand()%4)+1;
		}
	} 

}




