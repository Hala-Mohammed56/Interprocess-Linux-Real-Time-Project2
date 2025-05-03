// main.c
#include "ipc_setup.h"
#include "shared_defs.h"
#include "shm_ingredients.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"

#define SHM_NAME_STORE "/bakery_store"

int main()
{
    printf(" Initializing IPC Resources...\n");

    //  Create oven semaphore
    init_oven_semaphore();
    printf(" Oven semaphore created.\n");

    //  Initialize ingredient shared memory and

    semaphore
        init_ingredient_ipc(1); // 1 = create
    sem_t *sem = sem_open(SEM_INGREDIENTS_NAME, 0);
    if (sem == SEM_FAILED)
    {
        perror(" Failed to open SEM_INGREDIENTS_NAME");
        exit(1);
    }
    else
    {
        sem_close(sem);
        printf("Ingredient semaphore is ready.\n");
    }

    printf("Ingredients shared memory and semaphore initialized.\n");

    //  Initialize shared memory for BakerySharedData
    BakerySharedData *shm = (BakerySharedData *)init_shared_memory(SHM_NAME_STORE, sizeof(BakerySharedData));
    printf(" Bakery shared memory created.\n");

    //  Set initial values
    memset(shm, 0, sizeof(BakerySharedData)); // clear everything to 0

    shm->oven.ovens_in_use = 0;
    shm->ready.bread_ready = 0;
    shm->ready.cake_ready = 0;
    shm->ready.sweets_ready = 0;
    shm->ready.patisserie_ready = 0;

    shm->sales.profit = 0;
    shm->sales.complaints = 0;
    shm->sales.frustrated_customers = 0;
    shm->sales.total_customers = 0;

    printf(" Shared memory initialized with default values.\n");

    //  Detach ingredient memory (optional cleanup)
    cleanup_ingredients();

    printf(" IPC setup complete. You may now launch the simulation.\n");
    return 0;
}
