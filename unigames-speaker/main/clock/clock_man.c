#include <stdio.h>
#include "esp_log.h"
#include "../time/datetime.h"
#include "../time/time_man.h"
#include "../lcd/lcd_man.h"
#include "../audio/audio_man.h"
#include <unistd.h>

static const char *TAG = "clock_man";

void wait(unsigned int seconds) {
    usleep(seconds * 1000000);
}

void tell_time(struct DateTime dt, audio_component_t player) {
    char* buffer;
    char temp[100];

    printf("%02d:%02d\n", dt.hour, dt.minute);
    sprintf(temp, "/sdcard/nl/clock/61_het_is_nu.mp3");
    buffer = temp;
    play_audio(&player, buffer);
    sprintf(temp, "/sdcard/nl/clock/%d_%d.mp3", dt.hour, dt.hour);
    sprintf(temp, "/sdcard/nl/clock/62_uur.mp3");
    if (dt.minute > 0) {
        sprintf(temp, "/sdcard/nl/clock/%d_%d.mp3", dt.minute, dt.minute);
    }
    ESP_LOGI(TAG, "tell_time");
}

void display_time(audio_component_t player) {
    char buffer[20];
    struct DateTime dt;

    lcd_centerwrite("Time:", 1, false);

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