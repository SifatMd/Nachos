#include "copyright.h"
#include "Table.h"
#include "synch.h"


Lock *tableLock;

Table::Table(int size){
      tableLock = new Lock("TableLock");
      tableSize = size;
      numProcess = 0;

      arrayOfPointers = new void*[tableSize+1];
      for(int i=0;i<=size;i++){
          arrayOfPointers[i] = NULL;
      }
}

Table::~Table(){
    delete tableLock;
    delete []arrayOfPointers;
}


int Table::Alloc(void *object){
      int val = -1;
      tableLock->Acquire();

      for(int i=1;i<=tableSize;i++){
            if(arrayOfPointers[i]==NULL){
                  arrayOfPointers[i] = object;
                  val = i;
                  numProcess++;
                  break;
            }
      }

      tableLock->Release();
      return val;
}



void* Table::Get(int index){
    tableLock->Acquire();

    if(arrayOfPointers[index]!=NULL){
        tableLock->Release();
        return arrayOfPointers[index];
    }

    tableLock->Release();
    return NULL;
}

void Table::Release(int index){
      tableLock->Acquire();

      arrayOfPointers[index] = NULL;
      numProcess--;

      tableLock->Release();

}

int Table::GetNumProcess(){
      return numProcess;
}
