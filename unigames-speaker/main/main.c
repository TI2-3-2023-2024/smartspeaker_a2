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

// low rate mp3 audio
extern const uint8_t lr_mp3_start[] asm("_binary_music_16b_2c_8000hz_mp3_start");
extern const uint8_t lr_mp3_end[]   asm("_binary_music_16b_2c_8000hz_mp3_end");

// medium rate mp3 audio
extern const uint8_t mr_mp3_start[] asm("_binary_music_16b_2c_22050hz_mp3_start");
extern const uint8_t mr_mp3_end[]   asm("_binary_music_16b_2c_22050hz_mp3_end");

// high rate mp3 audio
extern const uint8_t hr_mp3_start[] asm("_binary_music_16b_2c_44100hz_mp3_start");
extern const uint8_t hr_mp3_end[]   asm("_binary_music_16b_2c_44100hz_mp3_end");

void display() {
    display_time();
}

void app_main(void) {
    lcd_init();
    time_init();
    button_han_init();

    start_thread("display_time", display);

    // struct DateTime dt;
    // char buffer[20];

    // lcd_centerwrite("Time:", 1, false);

    // while (true) {
    //     dt = get_time();
    //     sprintf(buffer, "%02d:%02d", dt.hour, dt.minute);
    //     lcd_centerwrite(buffer, 2, false);

    //     if (dt.minute == 0 || dt.minute == 30) {
    //         tell_time(dt);
    //     }

    //     wait(60 - dt.second);
    // }
    
}
