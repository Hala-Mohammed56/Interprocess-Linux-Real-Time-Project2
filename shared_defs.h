#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#define SHM_KEY 0x2025  // Unique shared memory key

// 🧺 Store: raw ingredient stock (used by chefs)
typedef struct {
    int wheat;
    int yeast;
    int butter;
    int milk;
    int sugar;
    int salt;
    int sweet_items;
    int cheese;
    int salami;
} Store;

// ✅ ReadyItems: finished items ready for selling
typedef struct {
    int bread_ready;
    int cake_ready;
    int sweets_ready;
    int patisserie_ready;
} ReadyItems;

// 🔥 Oven: oven usage status
typedef struct {
    int ovens_in_use;
} Oven;

// 💵 Sales: profit + customer stats
typedef struct {
    double profit;
    int complaints;
    int frustrated_customers;
    int total_customers;
} Sales;

// 🧠 Full shared memory structure
typedef struct {
    Store store;           // Ingredients for chefs
    ReadyItems ready;      // Baked items for sellers
    Oven oven;             // Baking status
    Sales sales;           // Metrics
} BakerySharedData;

#endif // SHARED_DEFS_H
