#include "copyright.h"
#include "console.h"




class SynchConsole{

private:
    Console *console;


public:

    SynchConsole();

    ~SynchConsole();

    void GetLock();
    void ReleaseLock();

    char GetChar();
    void PutChar(char ch);


};
