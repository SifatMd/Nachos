#include "copyright.h"
#include "MemoryManager.h"
#include "synch.h"
#include "Table.h"

Lock *memoryLock;
extern Table *table;

MemoryManager::MemoryManager(int numPages){
	bitmap = new BitMap(numPages);
	memoryLock = new Lock("MemoryManager");
	timeStamp = 0;

	printf("Number of pages in MemoryManager: %d\n", numPages);

	totalLocation = numPages;
	freeLocation = numPages;

	groups = new Group[numPages];

	for(int i=0;i<numPages;i++){
			groups[i].isOccupied = false;
			groups[i].processNumber = -1;
			groups[i].entry = NULL;
			groups[i].lastUsed = 0;
	}

}


MemoryManager::~MemoryManager(){
		delete bitmap;
		delete groups;
		delete memoryLock;
}


int MemoryManager::AllocPage(){
	int num = -1;

	memoryLock->Acquire();
	num = bitmap->Find();
	memoryLock->Release();
	return num;
}


int MemoryManager::AllocPage(int processNo, TranslationEntry *entry){
		int val = -1;
		memoryLock->Acquire();

		if(freeLocation>0){
				for(int i=0;i<totalLocation;i++){
						if(groups[i].isOccupied==false){
									val = i;
									freeLocation--;
									DEBUG('t',"Group position located is %d\n", val);
									break;
						}
				}
		}
		else{
				val = AllocByForce();
		}

		groups[val].isOccupied = true;
		groups[val].processNumber = processNo;
		groups[val].entry = entry;

		memoryLock->Release();

		return val;

}


int MemoryManager::AllocByForce(){
			int xx = rand() % totalLocation; //random for now
			//printf("Randomly generated: %d\n", xx);

			//return 1;
			//printf("inside AllocByForce\n");
			DEBUG('t', "Inside AllocByForce\n");
			int location, minTime;
			minTime = groups[0].lastUsed;
			location = 0;

			for(int i=1;i<totalLocation;i++){
					if(minTime > groups[i].lastUsed){
								minTime = groups[i].lastUsed;
								location = i;
					}
			}

			//location = xx; //this line gives random replacement

			int pID = groups[location].processNumber; //process no of that process which used to occupy this page in physical memory

			Thread *process = (Thread*) table->Get(pID); //extracting that process thread from table

			TranslationEntry *te = groups[location].entry; //get the entry to find out virtpageno
			int virtPgNo = te->virtualPage;

			AddrSpace *space = process->space;

			//storing and evicting the page
			if(te->dirty){
					space->storeFile->Store(virtPgNo, te->physicalPage);
			}

			te->use = false;
			te->dirty = false;
			te->valid = false;

			//printf("Page Evicted: %d, Time: %llu\n", location, timeStamp+1);
			DEBUG('x', "Page Evicted: %d, Time: %llu\n", location, timeStamp+1);
			DEBUG('t',"Allocation by force of page %d\n", location);

			return location;

}



void MemoryManager::FreePage(int physPageNum){
	memoryLock->Acquire();
	bitmap->Clear(physPageNum);
	memoryLock->Release();
}


void MemoryManager::FreeLocation(int physPageNum){
		memoryLock->Acquire();

		if(groups[physPageNum].isOccupied){
				//printf("Freeing %d spot in memory manager\n", physPageNum);
				DEBUG('t', "Freeing %d spot in memory manager\n", physPageNum);
				groups[physPageNum].isOccupied = false;
				groups[physPageNum].processNumber = -1;
				groups[physPageNum].entry = NULL;
				freeLocation++;
		}

		memoryLock->Release();
		return ;
}

bool MemoryManager::PageIsAllocated(int physPageNum){
	bool a;
	memoryLock->Acquire();
	a = bitmap->Test(physPageNum);
	memoryLock->Release();

	return a;
}


bool MemoryManager::LocationIsAllocated(int physPageNum){
		memoryLock->Acquire();
		bool val = groups[physPageNum].isOccupied;
		memoryLock->Release();
		return val;

}



void MemoryManager::UpdateTime(int locNo){
		timeStamp++;
		groups[locNo].lastUsed = timeStamp;
}


int MemoryManager::AvailableMemory(){
	int cnt = 0;

	memoryLock->Acquire();
	cnt = bitmap->NumClear();
	memoryLock->Release();

	return cnt;
}


int MemoryManager::AvailableLocations(){
		int cnt = 0;

		memoryLock->Acquire();
		cnt = freeLocation;
		memoryLock->Release();

		return cnt;
}
