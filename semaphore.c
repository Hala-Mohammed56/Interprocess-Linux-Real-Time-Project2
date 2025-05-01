#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include "semaphore.h"

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

int create_semaphore(key_t key) {
    int id;

    id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (id != -1) {
        printf("[semaphore] created new, initializing to 1...\n");
        init_semaphore(id, 1);
        return id;
    }

    id = semget(key, 1, 0666);
    if (id == -1) {
        perror("semget failed");
        exit(1);
    }

    return id;
}


void init_semaphore(int semid, int value) {
    union semun arg;
    arg.val = value;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl init failed");
        exit(1);
    }
}

void sem_wait(int semid) {
    struct sembuf op = {0, -1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("sem_wait failed");
        exit(1);
    }
}

void sem_signal(int semid) {
    struct sembuf op = {0, 1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("sem_signal failed");
        exit(1);
    }
}
