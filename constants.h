#ifndef CONSTANTS_H
#define CONSTANTS_H

// Named FIFO paths
#define FIFO_CHEF_TO_BAKER     "/tmp/chef_to_baker"
#define FIFO_BAKER_TO_SELLER   "/tmp/baker_to_seller"
#define FIFO_SUPPLY_TO_CHEF    "/tmp/supply_to_chef"

// Named semaphore
#define OVEN_SEM_NAME          "/oven_semaphore"

// Sizes
#define MAX_ITEM_NAME_LENGTH   64
#define MAX_MESSAGE_SIZE       128

// constants.h
#define SHM_NAME_STORE "/bakery_store"


// Max number of bakers or chefs (for loops)
#define MAX_BAKERS             10
#define MAX_CHEFS              15

#endif
