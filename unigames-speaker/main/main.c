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

// low rate mp3 audio
extern const uint8_t lr_mp3_start[] asm("_binary_music_16b_2c_8000hz_mp3_start");
extern const uint8_t lr_mp3_end[]   asm("_binary_music_16b_2c_8000hz_mp3_end");

// medium rate mp3 audio
extern const uint8_t mr_mp3_start[] asm("_binary_music_16b_2c_22050hz_mp3_start");
extern const uint8_t mr_mp3_end[]   asm("_binary_music_16b_2c_22050hz_mp3_end");

// high rate mp3 audio
extern const uint8_t hr_mp3_start[] asm("_binary_music_16b_2c_44100hz_mp3_start");
extern const uint8_t hr_mp3_end[]   asm("_binary_music_16b_2c_44100hz_mp3_end");

void display(audio_component_t player) {
    display_time(player);
}

void app_main(void) {
    time_init();
    lcd_init();
    button_han_init();

#if defined CONFIG_ESP32_C3_LYRA_V2_BOARD
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_PDM_TX_CFG_DEFAULT();
#else
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
#endif

    audio_component_t player = init_audio(i2s_cfg);
    // char* bob = "file://sdcard/TIMMERCLUB.mp3";
    // play_audio(&player, bob);

   start_thread("display_time", display(player));
    
}
