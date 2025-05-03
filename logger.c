#include "shared_defs.h"
#include "ipc_setup.h"
#include <stdio.h>
#include <time.h>

void save_final_report()
{
    BakerySharedData *shm = (BakerySharedData *)attach_shared_memory(SHM_NAME_STORE, sizeof(BakerySharedData));
    FILE *report = fopen("final_report.txt", "w");
    if (!report)
    {
        perror("Failed to write final report");
        return;
    }

    time_t now = time(NULL);
    fprintf(report, "Final Simulation Report\n");
    fprintf(report, "Timestamp: %s\n", ctime(&now));
    fprintf(report, "----------------------------\n");
    fprintf(report, "Profit: $%.2f\n", shm->sales.profit);
    fprintf(report, "Complaints: %d\n", shm->sales.complaints);
    fprintf(report, "Frustrated Customers: %d\n", shm->sales.frustrated_customers);
    fprintf(report, "Total Customers: %d\n", shm->sales.total_customers);
    fprintf(report, "\nReady Items:\n");
    fprintf(report, "- Bread: %d\n", shm->ready.bread_ready);
    fprintf(report, "- Cake: %d\n", shm->ready.cake_ready);
    fprintf(report, "- Sweets: %d\n", shm->ready.sweets_ready);
    fprintf(report, "- Patisserie: %d\n", shm->ready.patisserie_ready);
    fprintf(report, "\nOvens in Use: %d\n", shm->oven.ovens_in_use);
    fclose(report);
}
