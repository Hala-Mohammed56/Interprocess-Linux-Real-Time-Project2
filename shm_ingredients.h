#ifndef SHM_INGREDIENTS_H
#define SHM_INGREDIENTS_H

#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#define SHM_INGREDIENTS_KEY 1234
#define SEM_INGREDIENTS_NAME "/sem_ingredients"

// Structure to hold ingredient quantities
typedef struct {
    int wheat;
    int yeast;
    int butter;
    int milk;
    int sugar_salt;
    int sweet_items;
    int cheese;
    int salami;
} Ingredients;

// Function prototypes
void init_ingredient_ipc(int create);
int try_consume_ingredients(const char* item);
void cleanup_ingredients();

#endif // SHM_INGREDIENTS_H
