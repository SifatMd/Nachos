#include "BackingStore.h"
#include "system.h"
#include <cmath>



BackingStore::BackingStore(int fileNo, int np){
    numPages = np;
    marker = new bool[numPages];
    for(int i=0;i<numPages;i++){
        marker[i] = false;
    }

    int fileSize = numPages * PageSize;

    char *nameOfFile = getName(fileNo);
    printf("BackingStore: Name of file is %s\n", nameOfFile);
    storeFileName = nameOfFile;

    fileSystem->Create(nameOfFile, fileSize);
    storeFile = fileSystem->Open(nameOfFile);


}


BackingStore::~BackingStore(){
    delete marker;
    delete storeFile;
    fileSystem->Remove(storeFileName);
    delete storeFileName;
}

void BackingStore::Store(int pageNumber, int location){
      marker[pageNumber] = true; //the page is being assigned against virtual page no
      char* src = machine->mainMemory + location * PageSize; //source is the actual location in physical RAM
      int dest = pageNumber * PageSize;

      storeFile->WriteAt(src, PageSize, dest);
}

void BackingStore::LoadFromStorage(int pageNumber, int location){
     char* dest = machine->mainMemory + location * PageSize;
     int src = pageNumber * PageSize;

     storeFile->ReadAt(dest, PageSize, src);

}


bool BackingStore::IsStored(int pageNumber){
    return marker[pageNumber];
}



char* BackingStore::getName(int fileNo){
    int num = fileNo;
    int a, b, cnt = 0, i, c;
    while(1){
        a = num/10;
        num = a;
        cnt++;
        if(a==0){
          break;
        }
    }
    num = fileNo;
    char *name = new char[cnt+1];
    cnt--;
    i=0;
    while(cnt!=-1){
        a = pow(10,cnt);
        b = num/a;
        c = num%a;
        num = c;
        name[i] = '0' + b;
        i++;
        cnt--;
    }
    name[i] = '\0';
    DEBUG('t',"The file number is %s\n", name);
    printf("The file number is %s\n", name);
    return name;
}
