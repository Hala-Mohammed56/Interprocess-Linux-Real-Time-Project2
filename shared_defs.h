#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#define SHM_KEY 0x1234

typedef struct {
    int wheat, yeast, butter, milk;
    int sugar_salt, sweet_items, cheese, salami;
} Store;

typedef struct {
    int sandwiches_ready;
    int cakes_ready;
    int sweets_ready;
    int chefs_status[16]; // 1 = busy, 0 = idle
} Kitchen;

typedef struct {
    int bread_in_oven;
    int patisseries_in_oven;
    int oven_status[4]; // 1 = busy, 0 = free
} Oven;

typedef struct {
    int customers_served;
    int customers_waiting;
    int complaints;
    int frustrated_customers;
} Sales;

typedef struct {
    Store store_data;
    Kitchen kitchen_data;
    Oven oven_data;
    Sales sales_data;
} SharedMemory;

#endif
