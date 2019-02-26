#include "copyright.h"
#include "SynchConsole.h"
#include "synch.h"


Lock *consoleLock;
Semaphore *readAvail;
Semaphore *writeDone;


void ReadAvail(void* arg){
    readAvail->V();

}

void WriteDone(void* arg){
    writeDone->V();

}


SynchConsole::SynchConsole(){
    consoleLock = new Lock("Console Lock");
    readAvail = new Semaphore("ReadAvail", 0);
    writeDone = new Semaphore("WriteDone", 0);

    console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
}


SynchConsole::~SynchConsole(){
      delete consoleLock;
      delete readAvail;
      delete writeDone;
      delete console;
}

void SynchConsole::GetLock(){
    consoleLock->Acquire();
}


void SynchConsole::ReleaseLock(){
    consoleLock->Release();
}


char SynchConsole::GetChar(){
    GetLock();

    readAvail->P();

    char ch = console->GetChar();

    ReleaseLock();

    return ch;
}


void SynchConsole::PutChar(char ch){
    GetLock();

    console->PutChar(ch);

    writeDone->P();

    ReleaseLock();

}
