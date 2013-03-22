// structure for the shared memory segment
#include<semaphore.h>

typedef struct sharedmem{
	
	// ...
	long int n,result;
	sem_t mutexflag;
	sem_t clientmutux;
	
	// ...
	
	// ...

} shmseg_t;
//shmseg_t sharedmemvariable = {-1,-1};

//semaphore setup
sem_t mutex;
	

