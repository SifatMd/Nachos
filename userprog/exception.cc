// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"
#include "machine.h"
#include "SynchConsole.h"
#include "Table.h"
#include "system.h"



//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

extern MemoryManager *memoryManager;
extern Table *table;
extern SynchConsole *synchConsole;
extern Statistics *stats;
long long int cnt = 0;

void SystemHaltHandler();
void SystemExecHandler();
void SystemExitHandler();
void SystemReadHandler();
void SystemWriteHandler();
void SystemPageFaultHandler();

void IncrementPC(){
    int pc = 0;

    pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);

    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);

    pc+=4;
    machine->WriteRegister(NextPCReg, pc);

}


void ForkFunction(void* val){
    //printf("New thread starts run %s.......\n", currentThread->getName());
    (currentThread->space)->InitRegisters();
    (currentThread->space)->RestoreState();
    machine->Run();
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
        DEBUG('e', "Inside Syscall Exception for thread %d\n", currentThread->threadID);
        DEBUG('e', "type is %d\n", type);

        if(type == SC_Halt){
            SystemHaltHandler();
        }
        else if(type == SC_Exec){
            SystemExecHandler();
            IncrementPC();
        }
        else if(type == SC_Exit){
            SystemExitHandler();
        }
        else if(type == SC_Read){
            SystemReadHandler();
            IncrementPC();
        }
        else if(type == SC_Write){
            //printf("Before write\n");
            SystemWriteHandler();
            //printf("After write\n");
            IncrementPC();
        }
    }
    else if(which == PageFaultException){
          //printf("PageFaultException occured...\n");
          cnt++;
          stats->numPageFaults++;
          SystemPageFaultHandler();
    }
    else if(which == ReadOnlyException){
          printf("ReadOnlyException occured...\n");
          SystemExitHandler();
    }
    else if(which == BusErrorException){
          printf("BusErrorException occured...\n");
          SystemExitHandler();
    }
    else if(which == AddressErrorException){
          printf("AddressErrorException occured...\n");
          SystemExitHandler();
    }
    else if(which == OverflowException){
          printf("OverflowException occured...\n");
          SystemExitHandler();
    }
    else if(which == IllegalInstrException){
          printf("IllegalInstrException occured...\n");
          SystemExitHandler();
    }
    else if(which == NumExceptionTypes){
          printf("NumExceptionTypes occured...\n");
          SystemExitHandler();
    }
    else {
      	printf("Unexpected user mode exception %d %d\n", which, type);
      	ASSERT(false);
    }

    //should be called to increment PC by 4
}


void SystemPageFaultHandler(){
    unsigned int faultVirtAddr = machine->ReadRegister(BadVAddrReg); //register 39 contains virt address for which page fault was generated
    unsigned int pageNo = faultVirtAddr / PageSize;
    unsigned int physicalPageNo = memoryManager->AllocPage(currentThread->threadID, &(machine->pageTable[pageNo]));

    currentThread->space->LoadIntoFreePage(faultVirtAddr, physicalPageNo);
}


void SystemHaltHandler(){
    DEBUG('e',"Shutdown By User Program...\n");
    interrupt->Halt();

    return;
}


void SystemExecHandler(){ //r4 contains buff
    //IncrementPC();

    char* fileName;
    int fileLength = 100;
    fileName = new char[fileLength];

    DEBUG('e', "Inside Execution");

    int i=0;
    int content = machine->ReadRegister(4);

    while(1){
        bool is = machine->ReadMem(content+i, 1, (int*)&fileName[i]);
        if(is ==false) machine->ReadMem(content+i, 1, (int*)&fileName[i]);
        if(fileName[i]=='\0'){
          break;
        }
        i++;
    }

    OpenFile *executable = fileSystem->Open(fileName);
    AddrSpace *space;

    //printf("filename is: %s\n", fileName);

    if(executable == NULL){
        printf("Unable to open file %s...\n", fileName);
        machine->WriteRegister(2, -1);
        return;
    }

    space = new AddrSpace(executable);

    Thread *newThread = new Thread(fileName);
    newThread->space = space;

    int val = table->Alloc((void*)newThread);
    newThread->threadID = val;
    if(val==-1){
        machine->WriteRegister(2, val);
        DEBUG('e', "Allocation failed...\n");
        printf("Allocation failed...\n");
        return ;
    }

    char *name = new char[10];
    name[0] = 'c';name[1] = 'h';name[2] = 'i';name[3] = 'l';name[4]='d';name[5]=' ';
    char ch = val+'0';
    name[6]=ch;

    newThread->setName(name);

    machine->WriteRegister(2, val);

    printf("SpaceID: %d\n", val);
    printf("Total threads: %d\n", table->GetNumProcess());
    printf("Name of child thread: %s\n", newThread->getName());

    //delete executable;
    delete fileName;


    newThread->Fork(ForkFunction, NULL);
    currentThread->Yield();
    DEBUG('e', "%d Inserted in Table...\n", newThread->threadID);

    return ;

}


void SystemExitHandler(){
    DEBUG('e', "Inside Exit");

    //printf("Inside exit\n");

    int status = machine->ReadRegister(4);

    printf("Exiting with status %d\n", status);
    printf("No of page fault exceptions: %d\n", stats->numPageFaults);

    currentThread->space->FreeUpMemory();
    delete currentThread->space;

    table->Release(currentThread->threadID);

    int cnt = table->GetNumProcess();
    if(cnt==0){
      interrupt->Halt();
    }

    //printf("number of process: %d\n", cnt);



    //currentThread->Yield();
    currentThread->Finish();



    DEBUG('e', "Exiting %d...\n", currentThread->threadID);
    //printf("Exiting thread %d...\n", currentThread->threadID);
    return ;

}


void SystemReadHandler(){
    //synchConsole->GetLock();
    //printf("dhur\n" );

    int addr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);

    char* buff = new char[size+1];
    //printf("size:: %d\n",size );
    for(int i=0;i<size;i++){
        buff[i] = synchConsole->GetChar();
    }

    buff[size] = '\0';

    for(int i=0;i<size;i++){
        bool ret = machine->WriteMem(addr+i, 1, (int)buff[i]);
        if(!ret) machine->WriteMem(addr+i, 1, (int)buff[i]);
    }
    //printf("Shesh\n" );

    machine->WriteRegister(2,size);

    DEBUG('e', "Inside Readhandler, size of string %d\n", size);

    //synchConsole->ReleaseLock();

    return ;
}


void SystemWriteHandler(){
    //synchConsole->GetLock();

    int addr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);

    //printf("Inside WriteHandler\n");


    for(int i=0;i<size;i++){
        int aa;
        bool ret = machine->ReadMem(addr+i, 1, &aa);
        if(!ret) machine->ReadMem(addr+i, 1, &aa);
        //printf("%c hoho",aa);
        synchConsole->PutChar((char)aa);


    }
    //printf("What\n");

    //synchConsole->PutChar('\n');

    DEBUG('e', "Inside WriteHandler...\n");

    //synchConsole->ReleaseLock();
    //printf("return\n" );
  //  while(1);
    return ;
}
