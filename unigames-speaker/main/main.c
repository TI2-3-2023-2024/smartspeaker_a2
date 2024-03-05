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

// low rate mp3 audio
extern const uint8_t lr_mp3_start[] asm("_binary_music_16b_2c_8000hz_mp3_start");
extern const uint8_t lr_mp3_end[]   asm("_binary_music_16b_2c_8000hz_mp3_end");

// medium rate mp3 audio
extern const uint8_t mr_mp3_start[] asm("_binary_music_16b_2c_22050hz_mp3_start");
extern const uint8_t mr_mp3_end[]   asm("_binary_music_16b_2c_22050hz_mp3_end");

// high rate mp3 audio
extern const uint8_t hr_mp3_start[] asm("_binary_music_16b_2c_44100hz_mp3_start");
extern const uint8_t hr_mp3_end[]   asm("_binary_music_16b_2c_44100hz_mp3_end");

void wait(int seconds);

void pressed(int a) { 
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
        break;
        case 8:
        printf("Deze knop heeft geen functie\n");
        break;
    }
} 

void app_main(void) {
    // lcd_init();
    button_han_init();

    // time_init();

    // lcd_write("test", 0, 0, false);

//     file_marker_t hr_mp3 = {
//         .start = hr_mp3_start,
//         .end = hr_mp3_end,
//     };

//     // get the i2s stream configuration
// #if defined CONFIG_ESP32_C3_LYRA_V2_BOARD
//     i2s_stream_cfg_t i2s_cfg = I2S_STREAM_PDM_TX_CFG_DEFAULT();
// #else
//     i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
// #endif

//     int a = 6969;
//     char* b = "sjoerd";
//     audio_component_t player = init_audio(i2s_cfg);
//     set_volume(&player, 100);
//     play_audio(&player, &hr_mp3);



    // struct DateTime dt;
    // char buffer[20];

    // lcd_centerwrite("Time:", 1, false);

    // while (true) {
    //     dt = get_time();
    //     sprintf(buffer, "%02d:%02d", dt.hour, dt.minute);
    //     lcd_centerwrite(buffer, 2, false);

    //     if (dt.minute == 0 || dt.minute == 30) {
    //         tell_time(dt.hour, dt.minute);
    //     }

    //     wait(60 - dt.second);
    // }
    
}

void wait(int seconds) {
    const TickType_t delay_ms = seconds * 1000;
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}
