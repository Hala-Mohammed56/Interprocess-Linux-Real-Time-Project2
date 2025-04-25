#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

// Struct that holds all simulation config values
typedef struct {
    // Products
    int num_bread_types;
    int num_sandwiches;
    int num_cake_flavors;
    int num_sweets;
    int num_sweet_patisseries;
    int num_savory_patisseries;

    // Employees
    int num_chefs;
    int num_bakers;
    int num_sellers;
    int num_supply_workers;

    // Ingredient stock ranges
    int wheat_min, wheat_max;
    int yeast_min, yeast_max;
    int butter_min, butter_max;
    int milk_min, milk_max;
    int sugar_salt_min, sugar_salt_max;
    int sweet_items_min, sweet_items_max;
    int cheese_min, cheese_max;
    int salami_min, salami_max;

    // Prices
    float price_bread;
    float price_sandwich;
    float price_cake;
    float price_sweet;
    float price_sweet_patisserie;
    float price_savory_patisserie;

    // Thresholds
    int max_frustrated_customers;
    int max_complaints;
    int max_missing_item_requests;
    float profit_goal;
    int simulation_duration_minutes;

    // Customer behavior
    int customer_arrival_interval_ms;
    int max_wait_time_ms;
    int complaint_chance_percent;
    int leave_if_unhappy_percent;

} Config;

// Function declaration to load config from file
Config load_config(const char* filename);

#endif
