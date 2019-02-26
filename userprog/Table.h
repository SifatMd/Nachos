#include "copyright.h"



class Table{

public:
        Table(int size);
        ~Table();

        int Alloc(void *object);
        void *Get(int index);
        void Release(int index);
        int GetNumProcess();

private:
        void** arrayOfPointers;
        int tableSize;
        int numProcess;

};
