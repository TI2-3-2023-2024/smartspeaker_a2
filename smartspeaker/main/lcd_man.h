#ifndef LCD_MAN_H
#define LCD_MAN_H

#include "hd44780.h"

static esp_err_t write_lcd_data(const hd44780_t *lcd, uint8_t data);
void lcd_test(void *pvParameters);
void lcd_main();

#endif