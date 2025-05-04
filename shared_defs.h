#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#define SHM_KEY 0x1234

typedef struct {
    int wheat, yeast, butter, milk;
    int sugar_salt, sweet_items, cheese, salami;
} Store;

typedef struct {
    int paste_ready;
    int cakes_ready;
    int sandwiches_ready;
    int sweets_ready;
    int sweet_patisseries_ready;
    int savory_patisseries_ready;

    int chefs_status[6]; // 1 = busy, 0 = idle (6 chefs)
} Kitchen;

typedef struct {
    int bread_in_oven;
    int cakes_in_oven;
    int sweets_in_oven;
    int sweet_patisseries_in_oven;
    int savory_patisseries_in_oven;
    // 0 = bread, 1 = sweet/savory patisseries, 2 = cakes/sweets
    int oven_status[3]; // 1 = busy, 0 = free
} Oven;

typedef struct {
    int customers_served;        // Used for statistics only
    int complaints;              // Used for statistics only
    int frustrated_customers;    // زبائن غادروا بسبب الطابور الكامل
    int daily_sales_dinar;       // مجموع دخل المبيعات بالدينار
} Sales;

typedef struct {
    int sandwich;
    int cake;
    int bread;
    int sweet;
    int sweet_patisserie;
    int savory_patisserie;
} Inventory;

typedef struct {
    Store store_data;
    Kitchen kitchen_data;
    Oven oven_data;
    Sales sales_data;          // Only for statistics
    Inventory inventory_data; // يحتوي على المنتجات الجاهزة
} SharedMemory;

#endif
