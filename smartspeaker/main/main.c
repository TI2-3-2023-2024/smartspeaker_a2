#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "lcd/lcd_man.h"
#include "wifi/wifi_man.h"

void app_main() {
    lcd_init();

    lcd_write("Hello World!", 0, 0, false);

    wifi_main();
}