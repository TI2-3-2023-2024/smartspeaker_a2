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
#include "audio/player.h"
#include "lcd/lcd_man.h"
#include "time/time_man.h"
#include "time/datetime.h"
#include "clock/clock_man.h"
#include "buttons/button_man.h"
#include "buttons/button_tasks.h"
#include "threads/thread_man.h"
#include "interface/user_interface.h"
#include "microphone/mic_man.h"

audio_component_t player;

void display() {
    display_time();
}

audio_component_t audio_init(void);
void audio_test(void);

void app_main(void) {
    lcd_init();
    //time_init();
    // mic_init();
    //player = init_audio();

    menu_start();
    button_han_init(handle_menu);

    //char *file_uris[] = {"/sdcard/nl/games/bas/HOHOHO.mp3", "/sdcard/nl/games/bas/JA.mp3", "/sdcard/nl/games/bas/HOHOHO.mp3", "/sdcard/nl/games/bas/JA.mp3", "/sdcard/nl/games/bas/JA.mp3"};
    //play_audio(&player, file_uris[0]);
}