/* Ref -> Andrew S Tannebaum, Herbert Bos - Modern Operating Systems */

/* used the code given in book */ 
/* added a main() function, that creates threads(philosophers) */ 
/* added functions for eat() and think() */ 
/* added a dispay function (displayStates()) that helps visualize the problem */
/* used POSIX threads and semaphores to implement the solution */

#define N 5                  /* number of philosophers */
#define LEFT (i + N - 1) % N /* number of i’s left neighbor */
#define RIGHT (i + 1) % N    /* number of i’s right neighbor */
#define THINKING 0           /* philosopher is thinking */
#define HUNGRY 1             /* philosopher is trying to get forks */
#define EATING 2             /* philosopher is eating */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int state[N];  /* array to keep track of everyone’s state */
sem_t mutex;   /* mutual exclusion for critical regions */
sem_t s[N];    /* one semaphore per philosopher */


void displayStates(int statesArr[], int num)
{

    for (int i = 0; i < num; i++)
    {
        printf("%i\t",statesArr[i]); 
        fflush(stdout);
    }
    printf("\n");
}


void test(int i) /* i: philosopher number, from 0 to N−1 */
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        displayStates(state,N);
        sem_post(&s[i]);
    }
}

void take_forks(int i)        /* i: philosopher number, from 0 to N−1 */
{
    sem_wait(&mutex);         /* enter critical region */
    state[i] = HUNGRY;        /* record fact that philosopher i is hungry */
    displayStates(state,N);
    test(i);                  /* try to acquire 2 forks */
    sem_post(&mutex);         /* exit critical region */
    sem_wait(&s[i]);          /* block if forks were not acquired */
}

void put_forks(int i)         /* i: philosopher number, from 0 to N−1 */
{
    sem_wait(&mutex);         /* enter critical region */
    state[i] = THINKING;      /* philosopher has finished eating */
    displayStates(state,N);
    test(LEFT);               /* see if left neighbor can now eat */
    test(RIGHT);              /* see if right neighbor can now eat */
    sem_post(&mutex);         /* exit critical region */
}

void think()
{
    sleep(rand() % 3 + 1); 
}

void eat()
{
    sleep(rand() % 2 + 1); 
}

void *philosopher(void* i)    /* i: philosopher number, from 0 to N−1 */
{
    int j = *(int*) i;
    while (1)
    {                  /* repeat forever */
        think();       /* philosopher is thinking */
        take_forks(j); /* acquire two forks or block */
        eat();         /* yum-yum, spaghetti */
        put_forks(j);  /* put both forks back on table */
    }
}

int main()
{ 

    sem_init(&mutex, 0, 1); 

    for (int a = 0; a < N; a++)
    {
        sem_init(&s[a], 0, 0);
    }

    printf("\n'P' is Philosopher\n'O' is Thinking, '1' is Hungry, '2' is Eating\n\n"); 
    printf("P\tP\tP\tP\tP\n"); 

    int i, j =0; 
    pthread_t thread_arr[N]; /* creating a thread of N philosophers */
    for (i = 0; i < N; i++)
    {
        int *b = (int *)malloc(sizeof(*b)); /* allocating memory to a variale to pass into a thread function */
        *b = i;
        pthread_create(&thread_arr[i], NULL, philosopher, b);
    }

    for (j = 0; j < N; j++)
    {
        pthread_join(thread_arr[j], NULL);
    }
    return 0; 
}