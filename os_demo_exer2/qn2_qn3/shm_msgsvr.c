#include <stdio.h>
#include <sys/mman.h>  /* shm_* stuff, and mmap() */
#include <sys/types.h>
#include <unistd.h> /* exit() etc */
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>/* for random() stuff */
#include <time.h>
#include"ipcd.h"


#define SHMOBJ_PATH         "/foo1423"
#define MAX_MSG_LENGTH      50			/* maximum length of the content of the message */
#define TYPES               8			/* how many types of messages we recognize (fantasy) */

struct msg_s {					/* message structure for messages in the shared segment */
	int type;
	char content[MAX_MSG_LENGTH];
};


int Fibonacci(int n) {
	if (n == 0) return 0;
	int a = 1, b = 1, c, i;
	for (i=3; i<=n; i++) {c = b; b += a; a = c; }
	return b;
}

int main(int argc, char *argv[]) {


	char logmgr[30];
	pid_t pid, sid;
	char *args[10];
	printf("going to start logmgr \n");
	pid = fork();
	if(pid == 0)
	{
		sprintf(logmgr,"/home/avinashgs/os_demo_exer2/qn2_qn3/check.sh");
		execv(logmgr,args);
	}	
    

	int shmfd;
	int shared_seg_size = (1 * sizeof(shmseg_t));   /* want shared segment capable of storing 1 message */
	/* the shared segment, and head of the messages list */
	shmseg_t *shared_msg;

	/* requesting the removal of the shm object     --  shm_unlink() */
	if (shm_unlink(SHMOBJ_PATH) != 0) {
		// perror("In shm_unlink()");
		printf("the shared memory doesnt exist");
		// exit(1);
	}

	/* creating the shared memory object    --  shm_open()  */
	shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	//  shmfd = shm_open(SHMOBJ_PATH, O_RDWR, S_IRWXU | S_IRWXG);
	if (shmfd < 0) {
		perror("In shm_open()");
		exit(1);
	}
	fprintf(stderr, "Created shared memory object %s\n", SHMOBJ_PATH);

	/* adjusting mapped file size (make room for the whole segment to map)      --  ftruncate() */
	ftruncate(shmfd, shared_seg_size);

	/* requesting the shared segment    --  mmap() */    
	shared_msg = (shmseg_t *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (shared_msg == NULL) {
		perror("In mmap()");
		exit(1);
	}
	fprintf(stderr, "Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);

	srandom(time(NULL));
	/* producing a message on the shared segment */

	sem_init(&(shared_msg->mutexflag), 0, 1);
	sem_init(&(shared_msg->clientmutux), 0, 1);  
	    
//	int nz_flag = 1;
	while(1)
	{      
//		nz_flag = 0;
		//   sem_wait(&(shared_msg->mutexflag));
//		if(shared_msg->n == -1 || (nz_flag!=0)){
		if(shared_msg->n == -1){
			printf("Client hasn't entered anything \n");
			sleep(3);
		}
		else	{
			printf("\nClient entered: %ld \n",shared_msg->n);

			shared_msg->result = Fibonacci(shared_msg->n);
			shared_msg->n = -1;
			printf("\nResult posted \n \n");	
			//    sem_post(&(shared_msg->mutexflag));
			//shared_msg->type = random() % TYPES;
			//snprintf(shared_msg->content, MAX_MSG_LENGTH, "My message, type %d, num %ld", shared_msg->type, random());

		}
		sem_post(&(shared_msg->mutexflag));
	}

	return 0;
}
