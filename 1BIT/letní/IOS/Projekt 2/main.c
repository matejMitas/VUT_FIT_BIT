// main.c
// Řešení IOS-DU2, 27.4.2017
// Autor: Matěj Mitaš, FIT
// Přeloženo: gcc 4.2.1
// Hlavní soubor pro vytváření procesů

#include "params.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include "params.h"

// Variables

// Pointers on shared memory
int *o_order; // operation order
int *inmates; // outside inmates
int *tutors; // outside tutors
int *all_tutors; // all tutors being generated
int *waiting_inmates; // inside inmates
int *leaving_tutors; // inside tutors
int *processes;

// Alocation for shared memory
int shm_o_order; // operation order
int shm_inmates; // outside inmates
int shm_tutors; // outside tutors
int shm_all_tutors; // all tutors being generated
int shm_waiting_inmates; // inside inmates
int shm_leaving_tutors; // inside tutors
int shm_processes;

// Semaphores

sem_t *s_mutex_print;
sem_t *s_barrier;
sem_t *s_mutex_barrier;
sem_t *s_mutex;
sem_t *s_tutor_leave;
sem_t *s_inmate_enter;

// File to write to

FILE *f;

/**
 Prints out result to the file
 
 @param item_count order of operation
 @param type adult or child
 @param state what to print out
 @param mode to print out additional info for waiting
 */

void message(int item_count, char type, char *state, char mode) {
    // enter critical section
    sem_wait(s_mutex_print);
    
    // increment value
    (*o_order)++;
    // print out the action
    if (mode == true) {
        fprintf(f, "%d\t\t: %c %d\t: waiting: %d: %d\n", *o_order, type, item_count, *tutors, *inmates);
    } else {
        fprintf(f, "%d\t\t: %c %d\t: %s\n", *o_order, type, item_count, state);
    }
    
    // clear the file
    fflush(f);
    
    sem_post(s_mutex_print);
}

/**
 Catches processes in order to group them
 
 */

void barrier(struct params par) {
    
    // count number of process that went through
    sem_wait(s_mutex_barrier);
    (*processes)++;
    sem_post(s_mutex_barrier);
    
    if ((*processes) == (par.adults + par.children)) {
        sem_post(s_barrier);
    }
    
    sem_wait(s_barrier);
    sem_post(s_barrier);
}

/**
 Main function for parent
 @param par parameters of the program
 @param index which adult are we talking about
 
 */

void tutor(struct params par, int index) {
    // generate start process
    message(index + 1, 'A', STARTED, false);
    
    //
    // ENTERING THE CENTER
    //
    
    sem_wait(s_mutex);
    // added adults in the centre
    message(index + 1, 'A', ENTER, false);
    (*tutors)++;
    
    // if there are any waiting children
    if ((*waiting_inmates)) {
        // if there are waiting more than three, sent the signals
        int n = ((*waiting_inmates) > 3) ? 3 : (*waiting_inmates);
        
        for (int i = 0; i < n; i++) {
            sem_post(s_inmate_enter);
        }
        
        (*waiting_inmates) -= n;
        (*inmates) += n;
    }
    
    sem_post(s_mutex);
    
    //
    // WORKING IN THE CENTER
    //
    
    if (par.awt > 0) {
        usleep(rand() % par.awt * 1000);
    }
    
    //
    // (TRYING TO) END
    //

    sem_wait(s_mutex);
    message(index + 1, 'A', TLEAVE, false);
    
    if ((*inmates) <= (3 * ((*tutors) - 1))) {
        (*all_tutors)--;
        (*tutors)--;
        message(index + 1, 'A', LEAVE, false);
        sem_post(s_mutex);
    } else {
        (*leaving_tutors)++;
        message(index + 1, 'A', WAIT, true);
        sem_post(s_mutex);
        sem_wait(s_tutor_leave);
        
        // to ensure that 'leave' statement gets printed
        sem_wait(s_mutex);
        
        message(index + 1, 'A', LEAVE, false);
        (*all_tutors)--;
        (*tutors)--;
        
        sem_post(s_mutex);
    }
    
    //
    // ENDING SECTION
    //
    
    // wait for all processes to arrive here
    barrier(par);
    // finishing of the processes
    message(index + 1, 'A', FINISHED, false);
}

/**
 Main function for child
 @param par parameters of the program
 @param index which children are we talking about
 
 */

void inmate(struct params par, int index) {
    // generate start process
    message(index + 1, 'C', STARTED, false);
    
    //
    // ENTERING THE CENTER
    //
    
    sem_wait(s_mutex);
    
    // not tutors, move freely
    if ((*all_tutors) == 0) {
        (*inmates)++;
        message(index + 1, 'C', ENTER, false);
        sem_post(s_inmate_enter);
        sem_post(s_mutex);
    } else {
        if ((*inmates) < (3 * (*tutors))) {
            (*inmates)++;
            message(index + 1, 'C', ENTER, false);
            sem_post(s_mutex);
        } else {
            (*waiting_inmates)++;
            message(index + 1, 'C', WAIT, true);
            sem_post(s_mutex);
            sem_wait(s_inmate_enter);
            
            // add child to the play
            // print out added child
            sem_wait(s_mutex);
            (*inmates)++;
            message(index + 1, 'C', ENTER, false);
            sem_post(s_mutex);
        }
    }
    
    //
    // WORKING IN THE CENTER
    //
    
    if (par.cwt > 0) {
        usleep(rand() % par.cwt * 1000);
    }
    
    //
    // (TRYING TO) END
    //
    
    sem_wait(s_mutex);
    
    message(index + 1, 'C', TLEAVE, false);
    message(index + 1, 'C', LEAVE, false);

    (*inmates)--;
    
    if ((*leaving_tutors) && (*inmates) <= (3 * ((*tutors) - 1))) {
        (*leaving_tutors)--;
        (*all_tutors)--;
        (*tutors)--;
        sem_post(s_tutor_leave);
    }
    
    sem_post(s_mutex);
    
    //
    // ENDING SECTION
    //
    
    // wait for all processes to arrive here
    barrier(par);
    // finishing of the processes
    message(index + 1, 'C', FINISHED, false);
}

/**
 Clears out the allocated and opened resources
 
 */

void clear_res() {
    // unlink shared variables
    shmdt(o_order);
    shmdt(inmates);
    shmdt(tutors);
    shmdt(waiting_inmates);
    shmdt(leaving_tutors);
    shmdt(processes);
    shmdt(all_tutors);
    
    // close shared variables
    shmctl(shm_o_order, IPC_RMID, NULL);
    shmctl(shm_inmates, IPC_RMID, NULL);
    shmctl(shm_tutors, IPC_RMID, NULL);
    shmctl(shm_waiting_inmates, IPC_RMID, NULL);
    shmctl(shm_leaving_tutors, IPC_RMID, NULL);
    shmctl(shm_processes, IPC_RMID, NULL);
    shmctl(shm_all_tutors, IPC_RMID, NULL);
    
    // close and unlink semaphores
    sem_close(s_mutex_print);
    sem_close(s_mutex_barrier);
    sem_close(s_barrier);
    sem_close(s_mutex);
    sem_close(s_inmate_enter);
    sem_close(s_tutor_leave);
    
    // destroy semaphores
    sem_unlink(SEM_WRITE_MUTEX);
    sem_unlink(SEM_BARRIER_MUTEX);
    sem_unlink(SEM_BARRIER);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_INMATE_ENTER);
    sem_unlink(SEM_TUTOR_LEAVE);
    
    // close file
    fclose(f);
}

/**
 If program receives SIGTERM or SIGINT, it handles both events
 
 */

void ending () {
    clear_res();
    kill(getpid(), SIGTERM);
}


int main(int argc, const char * argv[]) {
    // spravne printovani
    setbuf(stdout, NULL);
    // Process ending handlers
    signal(SIGTERM, ending);
    signal(SIGINT, ending);
    
    // Params
    
    int status = 0;
    struct params par = read_params(argc, argv, &status);
    
    if (status == PARAM_ERROR) {
        return status;
    }
    
    // File
    
    f = fopen("proj2.out", "w+");
    
    if (f == NULL) {
        printf(">> Couldn't open file\n");
        return PARAM_ERROR;
    }

    // init random time
    srand(time(NULL));
    
    // Shared memory
    // Create shared memories
    shm_o_order = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_inmates = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_tutors = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_waiting_inmates = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_leaving_tutors = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_processes = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    shm_all_tutors = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    
    // open shared memories
    o_order = shmat(shm_o_order, NULL, 0);
    inmates = shmat(shm_inmates, NULL, 0);
    tutors = shmat(shm_tutors, NULL, 0);
    waiting_inmates = shmat(shm_waiting_inmates, NULL, 0);
    leaving_tutors = shmat(shm_leaving_tutors, NULL, 0);
    processes = shmat(shm_processes, NULL, 0);
    all_tutors = shmat(shm_all_tutors, NULL, 0);
    
    // set shared memory
    (*all_tutors) = par.adults;
    
    // error creating memory
    if(shm_o_order == -1            ||
       shm_inmates == -1            ||
       shm_tutors == -1             ||
       shm_waiting_inmates == -1    ||
       shm_all_tutors == -1         ||
       shm_leaving_tutors == -1     ||
       shm_processes == -1) {
        // funkce, ktera vycisti zdroje
        fprintf(stderr, ">> Shared memory segments couldn't been allocated\n");
        clear_res();
        return ALLOC_ERROR;
    }
    
    // Semaphores
    s_mutex_print = sem_open(SEM_WRITE_MUTEX, O_CREAT, S_IWUSR | S_IRUSR, 1);
    s_mutex_barrier = sem_open(SEM_BARRIER_MUTEX, O_CREAT, S_IWUSR | S_IRUSR, 1);
    s_barrier = sem_open(SEM_BARRIER, O_CREAT, S_IWUSR | S_IRUSR, 0);
    s_mutex = sem_open(SEM_MUTEX, O_CREAT, S_IWUSR | S_IRUSR, 1);
    s_inmate_enter = sem_open(SEM_INMATE_ENTER, O_CREAT, S_IWUSR | S_IRUSR, 0);
    s_tutor_leave = sem_open(SEM_TUTOR_LEAVE, O_CREAT, S_IWUSR | S_IRUSR, 0);
    
    // error of semaphores
    if(s_mutex_print == SEM_FAILED      ||
       s_mutex_barrier == SEM_FAILED    ||
       s_barrier == SEM_FAILED          ||
       s_mutex == SEM_FAILED            ||
       s_inmate_enter == SEM_FAILED     ||
       s_tutor_leave == SEM_FAILED) {
        // funkce, ktera vycisti zdroje
        fprintf(stderr, ">> Semaphores couldn't been opened\n");
        clear_res();
        return ALLOC_ERROR;
    }
    
    
    // Generators
    pid_t pid_inmate = 0;
    pid_t pid_tutor = fork();
    
    // child
    if (pid_tutor == 0) {
        // calling inmate generator
        pid_t pid_tutors[par.adults]; // array for five tutors
        
        for(int i = 0; i < par.adults; i++) {
            // fork process
            pid_t pid_tutor_process = fork();
            
            if (pid_tutor_process == 0) {
                // call tutor function if child
                tutor(par, i);
                exit(0);
                
            } else {
                // add to processes array
                pid_tutors[i] = getpid();
                // waiting to generate next process
                if (par.agt > 0) {
                    usleep(rand() % par.agt * 1000);
                }
            }
        }
        // wait for other processes to arrive
        for (int j = 0; j < par.adults; j++) {
            waitpid(pid_tutors[j], NULL, 0);
        }
    }
    
    else if (pid_tutor < 0) {
        // error handling for first fork
        fprintf(stderr, ">> Couldn't fork");
        clear_res();
        return ALLOC_ERROR;
    }
    
    // parent
    else {
        // fork for inmate
        pid_inmate = fork();
        
        if (pid_inmate == 0) {
            // store pids of the processes
            pid_t pid_inmates[par.children];
            
            for(int i = 0; i < par.children; i++) {
                
                pid_t pid_inmate_process = fork();
                
                if (pid_inmate_process == 0) {
                    
                    // call inmate function if child
                    inmate(par, i);
                    exit(0);
                    
                } else {
                    // add to processes array
                    pid_inmates[i] = getpid();
                    // waiting to generate next process
                    if (par.cgt > 0) {
                        usleep(rand() % par.cgt * 1000);
                    }
                }
                
            }
            // wait for other processes to arrive
            for (int j = 0; j < par.children; j++) {
                waitpid(pid_inmates[j], NULL, 0);
            }
        } else if (pid_inmate < 0) {
            // error handling for first fork
            fprintf(stderr, ">> Couldn't fork");
            clear_res();
            return ALLOC_ERROR;
        } else {
            // waiting for ending of the main process
            waitpid(pid_inmate, NULL, 0);
            // waiting for ending of the main process
            waitpid(pid_tutor, NULL, 0);
            // clear resources
            clear_res();
        }
    }
    
    return status;
}
