#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "lcd/lcd_man.h"
#include "wifi/wifi_man.h"
#include "wifi/datetime.h"

void app_main() {
    lcd_init();

    lcd_write("Dit is een voorbeeld", 0, 0, false);

    wifi_main();

    printf("test");
    struct DateTime dt = get_time();
    char buffer[20];  // Adjust the size as needed

    // Format the output into the buffer
    sprintf(buffer, "%d:%d", dt.hour, dt.minute);

    lcd_write(buffer, 0, 1, false);

}