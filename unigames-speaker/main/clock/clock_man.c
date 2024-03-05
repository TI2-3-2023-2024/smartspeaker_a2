#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "clock_man";

void tell_time(int hour, int minute) {
    printf("%02d:%02d\n", hour, minute);
    printf("61_het_is_nu.mp3\n");
    printf("%d_%d.mp3\n", hour, hour);
    printf("62_uur.mp3\n");
    if (minute > 0) {
        printf("%d_%d.mp3\n", minute, minute);
    }
    ESP_LOGI(TAG, "tell_time");
}