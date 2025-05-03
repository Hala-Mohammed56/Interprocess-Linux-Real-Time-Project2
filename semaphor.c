#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include "semaphor.h"

// Required for semctl with union semun
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

// Create (or get) a semaphore with the given key
int create_semaphore(key_t key) {
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("[Semaphore] semget failed");
        exit(EXIT_FAILURE);
    }
    return semid;
}

// Initialize the semaphore to a specific value
void init_semaphore(int semid, int value) {
    union semun arg;
    arg.val = value;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("[Semaphore] semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }
}

// Perform the wait (P) operation
void sem_wait(int semid) {
    struct sembuf op = {0, -1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("[Semaphore] sem_wait failed");
        exit(EXIT_FAILURE);
    }
}

// Perform the signal (V) operation
void sem_signal(int semid) {
    struct sembuf op = {0, 1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("[Semaphore] sem_signal failed");
        exit(EXIT_FAILURE);
    }
}
