#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "lcd/lcd_man.h"
#include "time/time_man.h"
#include "time/datetime.h"

void wait_for_60_seconds(void);

void app_main() {
    lcd_init();
    time_init();

    lcd_centerwrite("Time:", 0, false);

    struct DateTime dt = get_time();
    char buffer[20];  // Adjust the size as needed

    // Format the output into the buffer
    sprintf(buffer, "%d:%d", dt.hour, dt.minute);

    lcd_centerwrite(buffer, 1, false);

    wait_for_60_seconds();

    dt = get_time();

    // Format the output into the buffer
    sprintf(buffer, "%d:%d", dt.hour, dt.minute);

    lcd_centerwrite(buffer, 2, false);

}

void wait_for_60_seconds() {
    const TickType_t delay_ms = 60000; // 60 seconds in milliseconds
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}