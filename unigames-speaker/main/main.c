/* Play mp3 file by audio pipeline
   with possibility to start, stop, pause and resume playback
   as well as adjust volume

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "audio/audio_man.h"
#include "lcd/lcd_man.h"
#include "time/time_man.h"
#include "time/datetime.h"
#include "clock/clock_man.h"
#include "buttons/button_man.h"
#include "buttons/button_tasks.h"
#include "threads/thread_man.h"
#include "interface/user_interface.h"
#include "microphone/mic_man.h"

TaskHandle_t xHandle = NULL;

void test() {
    printf("Hello World\n");
}

void app_main(void) {
    time_init();
    //audio_component_t player = audio_init();
    lcd_init();

    // time_init();
    // audio_component_t player = audio_init();
    // lcd_init();
    mic_init();

    menu_start();
    button_han_init(handle_menu);

   // start_thread("display_time", display);

    // audio_test(player);

    set_volume(&player, 100);
    play_audio(&player, &hr_mp3);

    audio_component_t player = init_audio();
    set_volume(&player, 85);

    playlist_t *playlist = malloc(sizeof(playlist_t) + 2 * sizeof(char*));
    playlist->player = player;
    playlist->number_of_files = 3;
    playlist->on_finished = test;
    playlist->file_uris[0] = "/sdcard/peter.mp3";
    playlist->file_uris[1] = "/sdcard/TIMMERCLUB.mp3";

    xTaskCreate(play_multiple_audio_task, "play_multiple_audio_task", 8192, (void *) playlist, 5, &xHandle);
}
