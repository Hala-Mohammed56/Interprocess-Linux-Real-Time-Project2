#include "shm_ingredients.h"

static Ingredients *shm_ingredients = NULL;
static int shm_id = -1;
static sem_t *sem_ingredients = NULL;

// Initialize shared memory and semaphore
void init_ingredient_ipc(int create)
{
    if (create)
    {
        shm_id = shmget(SHM_INGREDIENTS_KEY, sizeof(Ingredients), IPC_CREAT | 0666);
    }
    else
    {
        shm_id = shmget(SHM_INGREDIENTS_KEY, sizeof(Ingredients), 0666);
    }

    if (shm_id < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    shm_ingredients = (Ingredients *)shmat(shm_id, NULL, 0);
    if (shm_ingredients == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    sem_ingredients = sem_open(SEM_INGREDIENTS_NAME, create ? O_CREAT : 0, 0666, 1);
    if (sem_ingredients == SEM_FAILED)
    {
        perror("sem_open failed");
        exit(1);
    }
}

// Try to consume ingredients (thread-safe with semaphore)
int try_consume_ingredients(const char *item)
{
    sem_wait(sem_ingredients);
    int success = 0;

    if (strcmp(item, "Bread Dough") == 0)
    {
        if (shm_ingredients->wheat >= 5 && shm_ingredients->yeast >= 2)
        {
            shm_ingredients->wheat -= 5;
            shm_ingredients->yeast -= 2;
            success = 1;
        }
    }
    else if (strcmp(item, "Cake Mixture") == 0)
    {
        if (shm_ingredients->milk >= 2 && shm_ingredients->butter >= 2 && shm_ingredients->sugar_salt >= 2)
        {
            shm_ingredients->milk -= 2;
            shm_ingredients->butter -= 2;
            shm_ingredients->sugar_salt -= 2;
            success = 1;
        }
    }
    else if (strcmp(item, "Sweet Patisserie Paste") == 0)
    {
        if (shm_ingredients->sweet_items >= 3 && shm_ingredients->butter >= 1)
        {
            shm_ingredients->sweet_items -= 3;
            shm_ingredients->butter -= 1;
            success = 1;
        }
    }
    else if (strcmp(item, "Savory Patisserie Paste") == 0)
    {
        if (shm_ingredients->cheese >= 2 && shm_ingredients->salami >= 2)
        {
            shm_ingredients->cheese -= 2;
            shm_ingredients->salami -= 2;
            success = 1;
        }
    }

    // NEW: Log missing request
    if (!success)
    {
        FILE *req = fopen("supply_requests.log", "a");
        if (req)
        {
            fprintf(req, "Request: %s (missing ingredients)\n", item);
            fclose(req);
        }
    }

    sem_post(sem_ingredients);
    return success;
}

// Cleanup shared resources
void cleanup_ingredients()
{
    if (shm_ingredients != NULL)
    {
        shmdt((void *)shm_ingredients);
        shm_ingredients = NULL;
    }

    if (sem_ingredients != NULL)
    {
        sem_close(sem_ingredients);
        sem_ingredients = NULL;
    }
}
