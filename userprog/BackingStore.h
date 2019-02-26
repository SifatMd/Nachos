#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H
#include "copyright.h"
#include "filesys.h"


class BackingStore{
  private:
    OpenFile *storeFile;
    char* storeFileName;
    bool *marker;
    int numPages;

  public:
    BackingStore(int fileNo, int np);
    ~BackingStore();

    void Store(int pageNumber, int location);
    void LoadFromStorage(int pageNumber, int location);
    bool IsStored(int pageNumber);
    char* getName(int fileNo);

};

#endif
