// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "list.h"	
#include "producer.h"
#include "consumer.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
#include "Abul.h"

Lock *lock;
Condition *P;
Condition *C;
List<int>* buffer;
int size;

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}


void Produce(void *name){
	char *n = (char*)name;
	Producer *p = new Producer(n, P, C, lock, buffer, size);
	p->produce();
}

void Consume(void *name){
	char *n = (char*)name;
	Consumer *c = new Consumer(n, P, C, lock, buffer, size);
	c->consume();

}

void Branch(){
	int i, j, k, loop;
	List<Thread*> *PThreads = new List<Thread*>;
	List<Thread*> *CThreads = new List<Thread*>;	

	lock = new Lock("LockOne");
	P = new Condition("Producer");
	C = new Condition("Consumer");
	buffer = new List<int>;
	size = 10;

	loop = 5;
	
	
	for(i=1;i<=loop;i++){
		char *name = new char[30];
		sprintf(name, "Producer %d", i);
		Thread *thread = new Thread(name);
		PThreads->Append(thread);
	}

	for(i=1;i<=loop;i++){
		char *name = new char[30];
		sprintf(name, "Consumer %d", i);
		Thread *thread = new Thread(name);
		CThreads->Append(thread);
	}
	
	for(i=1;i<=loop;i++){
		Thread *t1, *t2;
		t1 = PThreads->Remove();
		t2 = CThreads->Remove();
		
		t1->Fork((VoidFunctionPtr)Produce,(void*)t1->getName());
		t2->Fork((VoidFunctionPtr)Consume,(void*)t2->getName());
		
	}


}


//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new Thread("forked thread");

	Abul myAbul;
	//printf("%s\n", myAbul.getAbul());

	/*List<int> *list;
	list = new List;
	const char *name = "p";
	Condition *P = new Condition("producer");
	Condition *C = new Condition("consumer");
	Lock *lock = new Lock("only lock");

	int size = 10;*/
	
	
	Branch();

	//t->Fork((VoidFunctionPtr)SimpleThread,(void*)1);
	
	 
    //t->Fork(SimpleThread, 1);
    //SimpleThread(0);
	
}

