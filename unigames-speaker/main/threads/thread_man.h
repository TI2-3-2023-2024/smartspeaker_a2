#ifndef THREAD_MAN_H
#define THREAD_MAN_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> // For sleep()

// Define a structure to hold information about a thread
typedef struct ThreadInfo {
    char name[50];          // Thread name
    pthread_t thread_id;    // Thread ID
    int stop_flag;          // Flag to indicate if the thread should stop
    struct ThreadInfo* next;
} ThreadInfo;

int start_thread(const char* name, void* (*function)(void*));

#endif