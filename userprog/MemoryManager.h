#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "copyright.h"
#include "bitmap.h"
#include "Group.h"
#include "translate.h"



class MemoryManager{

	private:
		BitMap *bitmap;

		int totalLocation;
		int freeLocation;
		Group *groups;

		unsigned long long int timeStamp;

	public:

		MemoryManager(int numPages);
		~MemoryManager();

		int AllocPage();
		int AllocPage(int processNo, TranslationEntry *entry);
		int AllocByForce();

		void UpdateTime(int locNo);

		void FreePage(int physPageNum);
		void FreeLocation(int physPageNum);

		bool PageIsAllocated(int physPageNum);
		bool LocationIsAllocated(int physPageNum);

		int AvailableMemory();
		int AvailableLocations();

};

#endif
