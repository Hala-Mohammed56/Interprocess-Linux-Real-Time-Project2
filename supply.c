#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "config_loader.h"
#include "shared_defs.h"
#include "semaphore.h"

#define WAIT_SECONDS 12
#define N_ITEMS 8
#define SEM_KEY_STORE 0x8888

const char* item_names[N_ITEMS] = {
    "wheat", "yeast", "butter", "milk",
    "sugar_salt", "sweet_items", "cheese", "salami"
};

typedef struct {
    int waiting;
    time_t wait_start;
} ItemState;

int* get_field_pointer(Store* store, int index) {
    int* fields[N_ITEMS] = {
        &store->wheat, &store->yeast, &store->butter, &store->milk,
        &store->sugar_salt, &store->sweet_items, &store->cheese, &store->salami
    };
    return fields[index];
}

int get_min(Config* cfg, int index) {
    int mins[N_ITEMS] = {
        cfg->wheat_min, cfg->yeast_min, cfg->butter_min, cfg->milk_min,
        cfg->sugar_salt_min, cfg->sweet_items_min, cfg->cheese_min, cfg->salami_min
    };
    return mins[index];
}

int get_max(Config* cfg, int index) {
    int maxs[N_ITEMS] = {
        cfg->wheat_max, cfg->yeast_max, cfg->butter_max, cfg->milk_max,
        cfg->sugar_salt_max, cfg->sweet_items_max, cfg->cheese_max, cfg->salami_max
    };
    return maxs[index];
}

int random_between(int min, int max) {
    return min + rand() % (max - min + 1);
}

void restock(SharedMemory* shm, Config* cfg, ItemState* states, int semid) {
    Store* store = &shm->store_data;

    for (int i = 0; i < N_ITEMS; i++) {
        sem_wait(semid);
        int* val = get_field_pointer(store, i);
        int min = get_min(cfg, i);
        int max = get_max(cfg, i);

        if (*val <= 0) {
            if (!states[i].waiting) {
                states[i].waiting = 1;
                states[i].wait_start = time(NULL);
                printf("[SUPPLY] %s reached 0. Waiting %d seconds...\n", item_names[i], WAIT_SECONDS);
            } else {
                time_t now = time(NULL);
                if (now - states[i].wait_start >= WAIT_SECONDS) {
                    *val = random_between(min, max);
                    printf("[SUPPLY] Restocked %s to %d.\n", item_names[i], *val);
                    states[i].waiting = 0;
                }
            }
        } else {
            states[i].waiting = 0;
        }
        sem_signal(semid);
    }
}

int main() {
    srand(time(NULL));
    Config cfg = load_config("config.txt");

    // Create or get shared memory
    int shmid = shmget(SHM_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); exit(1); }

    SharedMemory* shm = (SharedMemory*)shmat(shmid, NULL, 0);
    if (shm == (void*)-1) { perror("shmat"); exit(1); }

    // Create semaphore
    int semid = create_semaphore(SEM_KEY_STORE);

    // 🔐 Protect initial filling from config
    sem_wait(semid);
    for (int i = 0; i < N_ITEMS; i++) {
        int min = get_min(&cfg, i);
        int max = get_max(&cfg, i);
        int* val = get_field_pointer(&shm->store_data, i);
        *val = random_between(min, max);
    }
    sem_signal(semid); // 🔓 Done filling

    // Start restock loop
    ItemState states[N_ITEMS] = {0};
    while (1) {
        restock(shm, &cfg, states, semid);
        sleep(1);
    }

    shmdt(shm);
    return 0;
}
