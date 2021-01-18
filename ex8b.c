//
//  main.c
//  OS_ex8
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

//-------------------------------------------------------------------------
#define FRONT_END 0
#define PRIMES 1
#define GCD 2
#define NUM_OF_THREADS 3
#define MAX_LEN 20

#define GCD_ACTION 'g'
#define PRIME_FACTORS_ACTION 'd'

//-------------------------------------------------------------------------
pthread_t threads_array[NUM_OF_THREADS] = {0};
// mutex semaphore
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_gcd = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_primes = PTHREAD_COND_INITIALIZER;

int gcd_arr[NUM_OF_THREADS] = {0}, prime_factors_arr[MAX_LEN] = {0};
//-------------------------------------------------------------------------
void terminate(char *);
void create_threads(void);
void *front_end_thread(void *arg);
void *gcd_thread(void *arg);
void *primes_thread(void *arg);
void sigint_handler(int signum);
void sigusr1_handler(int signum);
void wait_to_finish(void);
void initialize_prime_factors_arr(void);
void print_prime_factors(void);
void find_primary_factors(int);
int is_prime (int);
int find_gcd (int,int);
//-------------------------------------------------------------------------
int main() {

    signal(SIGINT, sigint_handler);
    create_threads();
    wait_to_finish();
    return EXIT_SUCCESS;
}
//=========================================================================
//this function terminate the program if there is an error in fork
void terminate(char *error_message){
    perror (error_message);
    exit(EXIT_FAILURE);
}
//-------------------------------------------------------------------------
void wait_to_finish(){
     int i;
     for(i = 0; i < NUM_OF_THREADS ; i++){
         if(pthread_join(threads_array[i], NULL)){
             terminate("pthread_join() failed\n");
         }
     }
 }
//-------------------------------------------------------------------------
// creates 3 servers threads & one frond end thread. exit if failed.
void create_threads(){
    //front end
    if (pthread_create(&(threads_array[FRONT_END]), NULL, front_end_thread, NULL)) terminate("front end failed\n");
    //gcd
    if(pthread_create(&(threads_array[GCD]), NULL, gcd_thread, NULL)) terminate("GCD failed\n");
    //prime factors
    if(pthread_create(&(threads_array[PRIMES]), NULL, primes_thread, NULL)) terminate("prime factors failed\n");
}
//---------------------------------------------------------------------------
void *front_end_thread(void *arg) {
    char get_action;
    signal(SIGINT, sigint_handler);
    signal(SIGUSR1, sigusr1_handler);
    
    while (1) {
        puts("insert g for GCD, d for Prime Factors");
        get_action = getc(stdin);
        getchar(); // to eat '\n'
        if( get_action == GCD_ACTION){
            printf("insert 2 nums:\n");
            scanf("%d",&gcd_arr[0]);
            scanf("%d",&gcd_arr[1]);
            if(pthread_mutex_unlock(&mutex)) terminate("failed to open semaphore");
            if(pthread_cond_signal(&cv_gcd)) terminate("failed to send signal");
            pause(); // waits for gcd thread
            printf("gcd is: %d\n", gcd_arr[2]);
            
        }else if (get_action == PRIME_FACTORS_ACTION){
            puts("picked PRIMES");
            scanf("%d",&prime_factors_arr[0]);
            if(pthread_mutex_unlock(&mutex)) terminate("failed to open semaphore");
            if(pthread_cond_signal(&cv_primes)) terminate("failed to send signal");
            pause(); // waits for primes thread
            print_prime_factors();
        } else break;
        getchar();
    }
    return NULL;
}
//-------------------------------------------------------------------------
void *gcd_thread(void *arg) {
    signal(SIGINT, sigint_handler);
    printf("GCD!\n");
    while(1){
        if(pthread_cond_wait(&cv_gcd, & mutex)) terminate("no signal");// wait for signal in order to run
        gcd_arr[2]=find_gcd(gcd_arr[0],gcd_arr[1]);
        printf("received: %d, %d\n",gcd_arr[0],gcd_arr[1]);
        pthread_kill(threads_array[0], SIGUSR1);
        if (pthread_mutex_unlock(& mutex)) terminate("mutex failure");
    }
    return NULL;
}
//---------------------------------------------------------------------------
void *primes_thread(void *arg)
{
    printf("%d\n", prime_factors_arr[0]);
    signal(SIGINT, sigint_handler);
    while(1) {
        if(pthread_cond_wait(&cv_primes, & mutex)) terminate("no signal");// wait for signal in order to run
        printf("%d\n", prime_factors_arr[0]);
        find_primary_factors(prime_factors_arr[0]);
        pthread_kill(threads_array[FRONT_END],SIGUSR1);
        if(pthread_mutex_unlock(& mutex)) terminate("mutex failure");
    }
    return NULL;
}
//---------------------------------------------------------------------------
void sigint_handler(int signum)
{
    printf("sigint sent\n");
    signal(SIGINT, sigint_handler);
    // condition destroyer
    pthread_cond_broadcast(&cv_gcd);
    pthread_cond_destroy(&cv_gcd);
    pthread_cond_broadcast(&cv_primes);
    pthread_cond_destroy(&cv_primes);
    //mutex destroyer
    pthread_mutex_destroy(&mutex);
    exit(EXIT_SUCCESS);
}
//---------------------------------------------------------------------------
void sigusr1_handler(int signum) {
    printf("sigusr1 was sent\n");
    if(pthread_mutex_lock(&mutex)) terminate("failed to lock front end thread");
}
//-------------------------------------------------------------------------
void find_primary_factors(int num){
    int i = 1,j ;
    initialize_prime_factors_arr();
    
    for (j = 2 ; j <= num && i < MAX_LEN ; j++){
        if(num % j == 0 && is_prime(j)){
            prime_factors_arr[i] = j;   //insert prime to array
            i++;
            num/=j;
            --j;
        }
    }
}
//-------------------------------------------------------------------------
int is_prime (int num){
    int i;
    for(i = 2 ; (i * i) <= num ; i++){
        if(num % i == 0)
            return 0;
    }
    return 1;
}
//-------------------------------------------------------------------------
void initialize_prime_factors_arr(){
    int i;
    for (i=1 ; i< MAX_LEN ; i++){
        prime_factors_arr[i]=-1;
    }
}
//-------------------------------------------------------------------------
void  print_prime_factors(){
    int i;
    for(i=1 ; prime_factors_arr[i]!=-1 ; i++){
        printf("%d ",prime_factors_arr[i]);
    }
    printf("\n");
}
//-------------------------------------------------------------------------
int find_gcd (int n1,int n2){
    int gcd = 0,i;
    for(i=1; i <= n1 && i <= n2; ++i)
        {
          // Checks if i is factor of both integers
          if(n1%i==0 && n2%i==0){
             gcd = i;
          }
        }
    return gcd;
}
//-------------------------------------------------------------------------
