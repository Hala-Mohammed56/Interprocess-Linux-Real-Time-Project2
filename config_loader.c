#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_loader.h"

#define LINE_SIZE 128

Config load_config(const char* filename) {
    Config cfg;
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }

    char line[LINE_SIZE];
    while (fgets(line, LINE_SIZE, file)) {
        if (line[0] == '#' || strlen(line) < 3) continue;

        char key[64], val[64];
        sscanf(line, "%[^=]= %s", key, val);

        // Helper macros to simplify parsing
        #define SET_INT(field) if (strcmp(key, #field) == 0) cfg.field = atoi(val)
        #define SET_FLOAT(field) if (strcmp(key, #field) == 0) cfg.field = atof(val)
        #define SET_RANGE(field) if (strncmp(key, #field, strlen(#field)) == 0) sscanf(val, "%d-%d", &cfg.field##_min, &cfg.field##_max)

        // Products
        SET_INT(num_bread_types);
        SET_INT(num_sandwiches);
        SET_INT(num_cake_flavors);
        SET_INT(num_sweets);
        SET_INT(num_sweet_patisseries);
        SET_INT(num_savory_patisseries);

        // Employees
        SET_INT(num_chefs);
        SET_INT(num_bakers);
        SET_INT(num_sellers);
        SET_INT(num_supply_workers);

        // Ingredient Ranges
        SET_RANGE(wheat);
        SET_RANGE(yeast);
        SET_RANGE(butter);
        SET_RANGE(milk);
        SET_RANGE(sugar_salt);
        SET_RANGE(sweet_items);
        SET_RANGE(cheese);
        SET_RANGE(salami);

        // Prices
        SET_FLOAT(price_bread);
        SET_FLOAT(price_sandwich);
        SET_FLOAT(price_cake);
        SET_FLOAT(price_sweet);
        SET_FLOAT(price_sweet_patisserie);
        SET_FLOAT(price_savory_patisserie);

        // Thresholds
        SET_INT(max_frustrated_customers);
        SET_INT(max_complaints);
        SET_INT(max_missing_item_requests);
        SET_FLOAT(profit_goal);
        SET_INT(simulation_duration_minutes);

        // Customer behavior
        SET_INT(customer_arrival_interval_ms);
        SET_INT(max_wait_time_ms);
        SET_INT(complaint_chance_percent);
        SET_INT(leave_if_unhappy_percent);
    }

    fclose(file);
    return cfg;
}
