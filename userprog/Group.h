#include "copyright.h"
#include "translate.h"


class Group{

public:
    bool isOccupied;
    int processNumber;
    TranslationEntry *entry;

    unsigned long long int lastUsed;
};
