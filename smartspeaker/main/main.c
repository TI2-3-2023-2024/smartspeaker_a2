#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "lcd/lcd_man.h"
#include "time/time_man.h"
#include "time/datetime.h"

void wait(int seconds);

void app_main() {
    lcd_init();
    time_init();

    struct DateTime dt;
    char buffer[20];

    lcd_centerwrite("Time:", 1, false);

    while(true) {
        dt = get_time();
        sprintf(buffer, "%02d:%02d", dt.hour, dt.minute);
        lcd_centerwrite(buffer, 2, false);
        wait(60 - dt.second);
    }

}

void wait(int seconds) {
    const TickType_t delay_ms = seconds * 1000;
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}