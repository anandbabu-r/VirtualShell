#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include"ipcd.h"

#define SHMOBJ_PATH         "/foo1423"      	/* Posix IPC object name [system dependant]*/
#define MAX_MSG_LENGTH      50			/* maximum length of the content of the message */
#define TYPES               8			/* how many types of messages we recognize (fantasy) */



#define MAXFIB               40


struct msg_s {					/* message structure for messages in the shared segment */
	int type;
	char content[MAX_MSG_LENGTH];
};

int main(int argc, char *argv[]) {


	int shmfd,i;
	int shared_seg_size = (1 * sizeof(shmseg_t));   /* want shared segment capable of storing 1 message */
	shmseg_t *shared_msg;			/* the shared segment, and head of the messages list */	
    
	sem_wait(&(shared_msg->clientmutux));   
        printf("\n Entering Critical Section of %d", atoi(argv[1]));
    
	if(argc == 2)
	{
	printf("N initial is %d \n", atoi(argv[1]));
	//shared_msg->n=atoi(argv[1]);
	}
	
	
	/* creating the shared memory object    --  shm_open()  */
	shmfd = shm_open(SHMOBJ_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    	if (shmfd < 0) {
		//perror("Server is busy with your previous request");
		printf("Server is busy with your previous request \n");
		exit(1);
    	}
	printf("Created shared memory object %s\n", SHMOBJ_PATH);
    
	/* requesting the shared segment    --  mmap() */    
	shared_msg = (shmseg_t *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (shared_msg == NULL) {
	perror("In mmap()");
	exit(1);
	}
	shared_msg->n = -1;

	//initialize semaphore
      

	printf("Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);

	if(argc == 2)
	{
		
		int n = atoi(argv[1]);
		printf("\n Client has entered : %d \n",n);		
		if( n < 0 ||   n >  MAXFIB  )
		{
			printf("\n Enter a  value between 0 and %d \n",MAXFIB);
			exit(1);
		}
		
	}
	else 
	{
		printf("\n Wrong usage: give N with the output file \n");
		exit(1);
	}

	shared_msg->n=atoi(argv[1]); 
	printf(" n is %ld \n", shared_msg->n);


	while(1)
	{
		sem_wait(&(shared_msg->mutexflag));   
		if(shared_msg->n == -1)	{
			printf("fiboananci(%d) is %ld \n",atoi(argv[1]),shared_msg->result);
			//shared_msg->n == -1;
			exit(0);
		}

		else	{   
		//printf("Waiting for Server:");
		} 
	
		sem_post(&(shared_msg->mutexflag));
		
	}

    
	sem_post(&(shared_msg->clientmutux));
	exit(0);

	return 0;
}

