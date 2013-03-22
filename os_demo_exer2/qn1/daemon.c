#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include<stdio.h>

int main(int argc, char *argv[]) {

char logmgr[30];
pid_t pid, sid;
char *args[10];	

pid = fork();

 
/* If the pid is less than zero,
 *    something went wrong when forking */
if (pid < 0) {
    exit(EXIT_FAILURE);
}

if(pid == 0)
	{
		printf("Entering Logmgr\n");
		sprintf(logmgr,"/home/avinashgs/os_demo_exer2/qn1/check.sh");
		execv(logmgr,args);
	}	
else
{	
printf("Entering Deamon\n");


/* If the pid we got back was greater
 *    than zero, then the clone was
 *       successful and we are the parent. */
/*if (pid > 0) {
    exit(EXIT_SUCCESS);
}*/

/* If execution reaches this point we are the child */
/* Set the umask to zero */


umask(0);



/* Open a connection to the syslog server */
openlog(argv[0],LOG_NOWAIT|LOG_PID,LOG_USER); 



/* Sends a message to the syslog daemon */
syslog(LOG_NOTICE, "Successfully started daemon\n"); 



/* Change the current working directory */
if ((chdir("/")) < 0) {
    syslog(LOG_ERR, "Could not change working directory to /\n");
    exit(EXIT_FAILURE);
}


/* Close the standard file descriptors */
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);

/* A useless payload to demonstrate */
while (1) {
    syslog(LOG_INFO, "My heart is beating...\n");
    sleep(15);
}

/* this is optional and only needs to be done when your daemon exits */
closelog();
}
}


