#ifndef HEADERS_H
#define HEADERS_H

// Common headers for all bakery simulation processes
// Contains standard and IPC-related includes

// Standard C Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// IPC-related
#include <sys/stat.h>     // For mkfifo
#include <sys/ipc.h>      // For IPC keys
#include <sys/shm.h>      // For shmget/shmat System V
#include <semaphore.h>    // For POSIX semaphores

#endif
