// ex8a2
// this program is the number creator for ex5a1
// here we create random numbers that will delete the
// numbers in the shared memory array.

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
//-------------------------------------------------------------------------
#define ARR_SIZE 1000
#define RAND_RANGE 200000
#define START_ARRAY 5
#define GLOBAL_COUNTER 4
//-------------------------------------------------------------------------
void terminate(char *);
int find_location(int *,int);
void delete_number(int *,int);
int can_while_run(int *);
//-------------------------------------------------------------------------
int main(int argc, const char * argv[]) {
    srand(atoi(argv[1]));
    key_t key;
    int shm_id, rand_num;
    int *shmem_ptr;
    int master_pid;
    int  location = 0, nums_created = 0, nums_deleted = 0;

    sem_t *s;
    s=sem_open("/my_mutex20",0);

    key = ftok(".", '8');
    shm_id = shmget(key, 0, 0600);
    if (shm_id == -1) {
        terminate("shm_id error!\n");
    }
    shmem_ptr = (int*) shmat(shm_id, NULL, 0);
    if (shmem_ptr == (int*) -1) {
        terminate("shmat failed\n");
    }

    shmem_ptr[atoi(argv[1])] = 1;
    master_pid = shmem_ptr[0];  // the master's pid
    // while all subprocesses are not = 1
    while (!can_while_run(shmem_ptr)) {}
//    sleep(1);
    shmem_ptr[GLOBAL_COUNTER] = 0;

    while (can_while_run(shmem_ptr)) {
        // if no more nums to delete
        if (shmem_ptr[GLOBAL_COUNTER]== ( ARR_SIZE - START_ARRAY)) {
            kill((pid_t)shmem_ptr[0], SIGUSR1);     //to the 'father'
            break;
        }
        sem_wait(s);
        //critic segment:
        rand_num = rand() % RAND_RANGE;
        nums_created++;
        location = find_location(shmem_ptr, rand_num);
        if (location > -1) {
            delete_number(shmem_ptr, location);
            nums_deleted++;
            shmem_ptr[GLOBAL_COUNTER]+=1;
        }
        sem_post(s); // is unlocked now
    }

    kill(master_pid, SIGUSR1);     //to the 'father'
    // to mark '0' in the cell- to stop brother processes.
    shmem_ptr[atoi(argv[1])] = 0;
    printf("%d %d\n", nums_created, nums_deleted);
    shmdt(shmem_ptr);
    sem_close(s);
    return EXIT_SUCCESS;
}
//=========================================================================
//this function terminate the program if there is an error in fork
void terminate(char *error_message){
    perror (error_message);
    exit(EXIT_FAILURE);
}
//-------------------------------------------------------------------------
int find_location(int arr[],int num){
    int i;
    for(i = START_ARRAY; i < ARR_SIZE ; i++){
        if(arr[i] == num)
            return i;
    }
    return -1;
}
//-------------------------------------------------------------------------
void delete_number(int* arr, int location){
    arr[location] = -1;
}
//-------------------------------------------------------------------------
// only to check the start. aftetr this, we use one of the cell as a global counter
// to stop when the array is comletley deleted.
int can_while_run(int *shmem_ptr){
    if ((shmem_ptr[1] == 1 ) && (shmem_ptr[2] == 1 ) && (shmem_ptr[3] == 1 )) {
        return 1;
    }
    return 0;
}
//-------------------------------------------------------------------------
