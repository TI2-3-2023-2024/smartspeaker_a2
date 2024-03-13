#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "../time/datetime.h"
#include "../time/time_man.h"
#include "../lcd/lcd_man.h"
#include "../audio/audio_man.h"

static const char *TAG = "clock_man";

TaskHandle_t xHandle = NULL;

void wait(unsigned int seconds) {
    usleep(seconds * 1000000);
}

void done() {
    ESP_LOGI(TAG, "tell_time");
}

void tell_time(struct DateTime dt, audio_component_t player) {
    int amount = 3;
    char** buffer = malloc(amount * sizeof(char*)); // Allocate memory for array of pointers
    if (buffer == NULL) {
        // Handle memory allocation failure
        return;
    }
    char temp[100];

    printf("%02d:%02d\n", dt.hour, dt.minute);
    buffer[0] = strdup("/sdcard/nl/clock/61_het_is_nu.mp3");
    buffer[1] = malloc(100); // Allocate memory for each buffer element
    sprintf(temp, "/sdcard/nl/clock/%d_%d.mp3", dt.hour, dt.hour);
    strcpy(buffer[1], temp); // Copy the string into allocated memory
    buffer[2] = strdup("/sdcard/nl/clock/62_uur.mp3");
    if (dt.minute > 0) {
        amount = 4;
        buffer[3] = malloc(100); // Allocate memory for the additional buffer element
        sprintf(temp, "/sdcard/nl/clock/%d_%d.mp3", dt.minute, dt.minute);
        strcpy(buffer[3], temp); // Copy the string into allocated memory
    }

    playlist_t *playlist = malloc(sizeof(playlist_t) + amount * sizeof(char*));
    if (playlist != NULL) {
        playlist->player = player;
        playlist->number_of_files = amount;
        playlist->on_finished = done;
        // Assign pointers to file URIs
        for (int i = 0; i < amount; ++i) {
            playlist->file_uris[i] = buffer[i];
        }
    }

    xTaskCreate(play_multiple_audio_task, "play_multiple_audio_task", 8192, (void *) playlist, 5, &xHandle);
}

void display_time(audio_component_t player) {
    char buffer[20];
    struct DateTime dt;

    // lcd_centerwrite("Time:", 1, false);

    while (true) {
        dt = get_time();
        sprintf(buffer, "%02d:%02d", dt.hour, dt.minute);
        lcd_centerwrite(buffer, 2, false);

        // if (dt.minute == 0 || dt.minute == 30) {
        //     tell_time(dt, player);
        // }
        tell_time(dt, player);

        wait(60 - dt.second);
    }
}