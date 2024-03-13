/* Play mp3 file by audio pipeline
   with possibility to start, stop, pause and resume playback
   as well as adjust volume

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "audio/audio_man.h"
#include "lcd/lcd_man.h"
#include "time/time_man.h"
#include "time/datetime.h"
#include "clock/clock_man.h"
#include "buttons/button_man.h"
#include "threads/thread_man.h"
#include "interface/user_interface.h"

#define PLAYLIST_SIZE 3

TaskHandle_t xHandle = NULL;

void display() {
    display_time();
}

void test() {
    printf("Finished\n");
}

audio_component_t audio_init(void);
void audio_test(audio_component_t player);

void app_main(void) {
    audio_component_t player = init_audio();

    playlist_t *playlist = malloc(sizeof(playlist_t) + PLAYLIST_SIZE * sizeof(char*));

    if (playlist != NULL) {
        playlist->player = player;
        playlist->number_of_files = 3;
        playlist->on_finished = test;
        playlist->file_uris[0] = "/sdcard/peter.mp3";
        playlist->file_uris[1] = "/sdcard/TIMMERCLUB.mp3";
        playlist->file_uris[2] = "/sdcard/goofy Ahh trap.mp3";
    }
    
    set_volume(&player, 85);
    
    xTaskCreate(play_multiple_audio_task, "play_multiple_audio_task", 8192, (void *) playlist, 5, &xHandle);
}