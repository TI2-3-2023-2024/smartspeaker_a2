#include "thread_man.h"

ThreadInfo* head = NULL;

int start_thread(const char* name, void* (*function)(void*)) {
    // Check if a thread with the same name already exists
    ThreadInfo* current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            printf("Thread with name '%s' already exists.\n", name);
            return -1;  // Thread with the same name already exists
        }
        current = current->next;
    }

    // Create a new thread
    ThreadInfo* new_thread = (ThreadInfo*)malloc(sizeof(ThreadInfo));
    if (new_thread == NULL) {
        printf("Memory allocation failed.\n");
        return -1;  // Memory allocation failed
    }
    strncpy(new_thread->name, name, sizeof(new_thread->name));
    new_thread->stop_flag = 0;  // Initialize stop flag
    if (pthread_create(&(new_thread->thread_id), NULL, function, (void*)new_thread) != 0) {
        printf("Failed to create thread '%s'.\n", name);
        free(new_thread);
        return -1;  // Failed to create thread
    }

    // Add the new thread to the linked list
    new_thread->next = head;
    head = new_thread;

    printf("Thread '%s' started successfully.\n", name);
    return 0;  // Success
}