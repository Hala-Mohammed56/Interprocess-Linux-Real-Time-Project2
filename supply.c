#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include "config_loader.h"

#define N_ITEMS 8
#define WAIT_SECONDS 15

const char* item_names[N_ITEMS] = {
    "wheat", "yeast", "butter", "milk",
    "sugar_salt", "sweet_items", "cheese", "salami"
};

typedef struct {
    int wheat, yeast, butter, milk, sugar_salt, sweet_items, cheese, salami;
} Store;

typedef struct {
    int waiting;        
    time_t wait_start;
} ItemState;

int* get_store_field(Store* store, int index) {
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

void restock_logic(Store* store, Config* cfg, ItemState* states) {
    for (int i = 0; i < N_ITEMS; i++) {
        int* val = get_store_field(store, i);
        int min = get_min(cfg, i);
        int max = get_max(cfg, i);

        if (*val <= 0) {
            if (!states[i].waiting) {
                states[i].waiting = 1;
                states[i].wait_start = time(NULL);
                printf("Item %s reached 0. Starting wait...\n", item_names[i]);
            } else {
                time_t now = time(NULL);
                if (now - states[i].wait_start >= WAIT_SECONDS) {
                    int new_stock = random_between(min, max);
                    *val = new_stock;
                    printf("Restocked %s to %d after waiting.\n", item_names[i], new_stock);
                    states[i].waiting = 0;
                }
            }
        } else {
            states[i].waiting = 0;
        }
    }
}

int main() {
    srand(time(NULL));

    Config cfg = load_config("config.txt");

    int shmid = shmget(2222, sizeof(Store), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); return 1; }

    Store* store = (Store*)shmat(shmid, NULL, 0);
    if (store == (void*)-1) { perror("shmat"); return 1; }

    ItemState states[N_ITEMS] = {0};

    while (1) {
        restock_logic(store, &cfg, states);
        sleep(1);
    }

    shmdt(store);
    return 0;
}
