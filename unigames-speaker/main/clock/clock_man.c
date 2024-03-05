#include <stdio.h>
#include "esp_log.h"
#include "../time/datetime.h"

static const char *TAG = "clock_man";

void tell_time(struct DateTime dt) {
    printf("%02d:%02d\n", dt.hour, dt.minute);
    printf("61_het_is_nu.mp3\n");
    printf("%d_%d.mp3\n", dt.hour, dt.hour);
    printf("62_uur.mp3\n");
    if (dt.minute > 0) {
        printf("%d_%d.mp3\n", dt.minute, dt.minute);
    }
    ESP_LOGI(TAG, "tell_time");
}