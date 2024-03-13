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
#include "buttons/button_tasks.h"
#include "threads/thread_man.h"
#include "interface/user_interface.h"


audio_component_t player;

void display() {
    display_time(player);
}

void react(int a) {
    switch(a){
        case 1:
        printf("tab omhoog\n");
        break;
        case 2:
        printf("terug\n");
        break;
        case 3:
        printf("ENTER\n");
        break;
        case 4:
        printf("tab omlaag\n");
        break;
        case 5:
        printf("volume omlaag\n");
        break;
        case 6:
        printf("volume omhoog\n");
        break;
        case 7:
        printf("Klok afspelen\n");
        tell_time(get_time(), player);
        break;
        case 8:
        printf("Deze knop heeft geen functie\n");
        break;
    } 
}
audio_component_t audio_init(void);
void audio_test(audio_component_t player);

void app_main(void) {
    time_init();
    lcd_init();
    button_han_init(react);

#if defined CONFIG_ESP32_C3_LYRA_V2_BOARD
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_PDM_TX_CFG_DEFAULT();
#else
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
#endif

    player = init_audio(i2s_cfg);
    set_player(player);
    set_volume(&player, 100);

    // start_thread("display_time", display);
}