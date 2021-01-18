//  OS_ex8
// OS- semester A- class B2
// Bar Ifrah- ID 315566810 login:- barif
// Sharon Levi ID 311593313 login:- sharonlevi
// Date - January 18th, 2021
//ex8a1
// master process
// this program creates the random numbers array and the shared memory.
// after checking that we have 3 different number makers, we look for
// numbers to delete. this is all done by the number finders.
// when the array is all set to be -1, we finish .
// the output will be- how much time it took.
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

//-------------------------------------------------------------------------
#define ARR_SIZE 1000
#define START_ARRAY 5
#define RAND_RANGE 200000
#define GLOBAL_COUNTER 4

//-------------------------------------------------------------------------
int did_enter_signal_catcher = 0;
//-------------------------------------------------------------------------
void terminate(char *);
int open_shared_memory(key_t);
int *open_shm_ptr(int);
void set_start(int *);
void build_array(int *);
void can_i_sleep(int *);
void can_print(int);
//-------------------------------------------------------------------------
int main() {
    srand(0);
    key_t key;
    signal(SIGUSR1, can_print);
    int shm_id;
    int *shmem_ptr;
    time_t time_start, time_finish;

    sem_t *s; //create a semaphore
   // sem_unlink("/my_mutex20");
    s=sem_open("/semaphore1",O_CREAT,0600,3);
    if(s==SEM_FAILED)
        	terminate("cannot open semaphore\n");

    key = ftok(".", '8');   //defined in ex.
    shm_id = open_shared_memory(key);
    shmem_ptr = open_shm_ptr(shm_id);
    set_start(shmem_ptr);
    build_array(shmem_ptr);

    can_i_sleep(shmem_ptr);         // checks 3 subprocesses array cells, abd sends signal

    time_start = time(NULL);
    if (!did_enter_signal_catcher) {
        pause();
    }
    time_finish = time(NULL);       // finished

    // stands by until all 3 subprocesses finishes.
    while(shmem_ptr[1] == 1 || shmem_ptr[2] == 1 || shmem_ptr[3] == 1 ) {}
    shmdt(shmem_ptr);
    if (shmctl(shm_id,IPC_RMID, NULL) == -1) {
        terminate("shmctl failed\n");
    }
    printf("%ld\n", (time_finish - time_start));

    sem_close(s);
    sem_unlink("/my_mutex20");
    return EXIT_SUCCESS;
}
//=========================================================================
//this function terminate the program if there is an error in fork
void terminate(char *error_message){
    perror (error_message);
    exit(EXIT_FAILURE);
}
//-------------------------------------------------------------------------
// opens the shared memory for the program
int open_shared_memory(key_t key){
    int shm_id;
    shm_id = shmget (key, ARR_SIZE, IPC_CREAT | IPC_EXCL | 0600) ;
    if (shm_id == -1){
        terminate("shared memory id error\n");
    }
    return shm_id;
}
//-------------------------------------------------------------------------
// opens and returns a pointer
int *open_shm_ptr(int shm_id){
    int *shmem_ptr;
    shmem_ptr = (int*) shmat(shm_id, NULL, 0);
    if (shmem_ptr == (int*) -1) {
        terminate("shmat failed\n");
    }
    return shmem_ptr;
}
//-------------------------------------------------------------------------
// set 0 on all starting array
void set_start(int *shmem_ptr){
    int i = 0;
    shmem_ptr[i] = getpid();
    for (i = 1; i < START_ARRAY; i++) {
        shmem_ptr[i] = 0;
    }
}
//-------------------------------------------------------------------------
// builds the randomized array
void build_array(int *random_arr){
    int i, rand_num;
    for (i = START_ARRAY; i < ARR_SIZE; i++) {
        rand_num = rand() % RAND_RANGE;
        random_arr[i] = rand_num;
    }
}
//-------------------------------------------------------------------------
// checks if all subprocessed are running to send this process to sleep
void can_i_sleep(int *shmem_ptr){
    int i = 1;
    while((i < GLOBAL_COUNTER)){
        if (shmem_ptr[i] == 1) {
            i++;
        }
    }
}
//-------------------------------------------------------------------------
// this to see if signal was caught.
// if it was- will not send this process to sleep
void can_print(int signal_num){
    did_enter_signal_catcher = 1;
}
//-------------------------------------------------------------------------
