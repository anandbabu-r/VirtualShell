#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>  /* shm_* stuff, and mmap() */
#include <unistd.h> /* exit() etc */
#include <sys/stat.h>
#include <stdlib.h>/* for random() stuff */


//CLOCK UTILS STARTS HERE
//TO CALCULATE DURATION OF EXECUTION

#ifndef __USE_POSIX199309
#define __USE_POSIX199309
#include <time.h>
#undef __USE_POSIX199309
#else
#include <time.h>
#endif

typedef struct timespec Timer;

static inline int initTimer(Timer* timer){
	clock_gettime(CLOCK_MONOTONIC,timer);
}

static inline double getTimer(Timer* timer){
	struct timespec endTimespec;
	clock_gettime(CLOCK_MONOTONIC,&endTimespec);
	return (endTimespec.tv_sec-timer->tv_sec) + (endTimespec.tv_nsec-timer->tv_nsec)*1e-9;
}

//CLOCK UTILS ENDS HERE


#define MAX_THREAD 20                         /* number of threads */
#define NDIM 100                              /* number of NDIMs */
double          a[NDIM][NDIM];
double          b[NDIM][NDIM];
double          c[NDIM][NDIM];

typedef struct
{
	int             id;                       /* identification */
	int             noproc;                   /* process */
	int             dim;                      /* number of threads */
	double	(*a)[NDIM][NDIM],(*b)[NDIM][NDIM],(*c)[NDIM][NDIM];
} parm;

void mm(int me_no, int noproc, int n, double a[NDIM][NDIM], double b[NDIM][NDIM], double c[NDIM][NDIM])
{
	int             i,j,k;
	double sum;
	i=me_no;
	while (i<n) {

		for (j = 0; j < n; j++) {
			sum = 0.0;
			for (k = 0; k < n; k++) {
				sum = sum + a[i][k] * b[k][j];
			}
			c[i][j] = sum;

		}
		i+=noproc;
	}
}

void * worker(void *arg)
{
	parm           *p = (parm *) arg;
	mm(p->id, p->noproc, p->dim, *(p->a), *(p->b), *(p->c));
}


void main(int argc, char *argv[])
{
	int             j, k, noproc, me_no;
	double          sum;
	double          t1, t2;

	pthread_t      *threads;

	parm           *arg;
	int             n, i;

	for (i = 0; i < NDIM; i++)
		for (j = 0; j < NDIM; j++)
		{
			a[i][j] = i + j;
			b[i][j] = i + j;
		}

	if (argc != 2)
	{
		printf("Issue a value to 'n' in the form: \n"
		"%s n:  where n is no. of thread\n", argv[0]);
		exit(1);
	}
	n = atoi(argv[1]);

	if ((n < 1) || (n > MAX_THREAD))
	{
		printf("The no of threads should between 1 and %d.\n", MAX_THREAD);
		exit(1);
	}
	threads = (pthread_t *) malloc(n * sizeof(pthread_t));

	arg=(parm *)malloc(sizeof(parm)*n);
	/* setup barrier */

	Timer timer;
	initTimer(&timer);

	/* Start up thread */
	/* Spawn thread */
/*
 int pthread_create(pthread_t *restrict thread,
         const pthread_attr_t *restrict attr, void *(*start_routine)(void *),
         void *restrict arg);

The pthread_create() function is used to create a new thread, with
     attributes specified by attr, within a process.  If attr is NULL, the
     default attributes are used.  If the attributes specified by attr are
     modified later, the thread's attributes are not affected.  Upon success-
     ful completion, pthread_create() will store the ID of the created thread
     in the location specified by thread.

     Upon its creation, the thread executes start_routine, with arg as its
     sole argument.  If start_routine returns, the effect is as if there was
     an implicit call to pthread_exit(), using the return value of
     start_routine as the exit status.  Note that the thread in which main()
     was originally invoked differs from this.  When it returns from main(),
     the effect is as if there was an implicit call to exit(), using the
     return value of main() as the exit status.

     The signal state of the new thread is initialized as:

           o   The signal mask is inherited from the creating thread.

           o   The set of signals pending for the new thread is empty.

 If successful,  the pthread_create() function will return zero.  Other-
     wise, an error number will be returned to indicate the error.
*/	

	for (i = 0; i < n; i++)
	{
		arg[i].id = i;
		arg[i].noproc = n;
		arg[i].dim = NDIM;
		arg[i].a = &a;
		arg[i].b = &b;
		arg[i].c = &c;
		pthread_create(&threads[i], NULL, worker, (void *)(arg+i));
	}

/*
int
     pthread_join(pthread_t thread, void **value_ptr);


DESCRIPTION

     The pthread_join() function suspends execution of the calling thread
     until the target thread terminates, unless the target thread has already
     terminated.

     On return from a successful pthread_join() call with a non-NULL value_ptr
     argument, the value passed to pthread_exit() by the terminating thread is
     stored in the location referenced by value_ptr.  When a pthread_join()
     returns successfully, the target thread has been terminated.  The results
     of multiple simultaneous calls to pthread_join(), specifying the same
     target thread, are undefined.  If the thread calling pthread_join() is
     cancelled, the target thread is not detached.


RETURN VALUES

     If successful,  the pthread_join() function will return zero.  Otherwise,
     an error number will be returned to indicate the error.



*/


	for (i = 0; i < n; i++)
	{
		pthread_join(threads[i], NULL);

	}
	
	double duration = getTimer(&timer);
	printf("Duration of execution = %le seconds \n",duration);
//	print_matrix(NDIM);
	check_matrix(NDIM);
	free(arg);
}

print_matrix(dim)
int dim;
{
	int i,j;

	printf("The %d * %d matrix is\n", dim,dim);
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++)
			printf("%lf ",  c[i][j]);
		printf("\n");
	}
}


/*check if the result of matrix multiplication is correct*/

check_matrix(dim)
int dim;
{
	int i,j,k;
	int error=0;

	printf("Now checking the results\n");
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++) {
			double e=0.0;

			for (k=0;k<dim;k++)
				e+=a[i][k]*b[k][j];

			if (e!=c[i][j]) {
				printf("(%d,%d) error\n",i,j);
				error++;
			}
		}
	if (error)
		printf("%d elements error\n",error);
		else
		printf("Matrix multiplication and verification of results is successful\n");
}
