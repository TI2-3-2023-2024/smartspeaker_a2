#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/time.h>
#include <hd44780.h>
#include <pcf8574.h>
#include <string.h>

static i2c_dev_t pcf8574;
static hd44780_t lcd;

#define HD44780_ROWS 4
#define HD44780_COLS 20

static uint32_t get_time_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

static const uint8_t char_data[] = {
    0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00,
    0x1f, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x1f, 0x00
};

static esp_err_t write_lcd_data(const hd44780_t *lcd, uint8_t data) {
    return pcf8574_port_write(&pcf8574, data);
}

void lcd_initialize() {
    // Set up the LCD configuration
    lcd = (hd44780_t) {
        .write_cb = write_lcd_data,
        .font = HD44780_FONT_5X8,
        .lines = 4,
        .pins = {
            .rs = 0,
            .e  = 2,
            .d4 = 4,
            .d5 = 5,
            .d6 = 6,
            .d7 = 7,
            .bl = 3
        }
    };

    // Initialize the LCD
    memset(&pcf8574, 0, sizeof(i2c_dev_t));
    ESP_ERROR_CHECK(pcf8574_init_desc(&pcf8574, CONFIG_EXAMPLE_I2C_ADDR, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(hd44780_init(&lcd));
    hd44780_switch_backlight(&lcd, true);

    // Upload custom characters if needed
    hd44780_upload_character(&lcd, 0, char_data);
    hd44780_upload_character(&lcd, 1, char_data + 8);
}

void lcd_clear(uint8_t line) {
    // Position the cursor to the start of the line
    hd44780_gotoxy(&lcd, 0, line);

    // Write spaces to clear the line
    for (int i = 0; i < HD44780_COLS; i++) {
        hd44780_putc(&lcd, ' '); // Write a space character
    }

    // Reset the cursor to the start of the line
    hd44780_gotoxy(&lcd, 0, line);
}


void lcd_fullclear() {
    for (int line = 0; line < HD44780_ROWS; line++) {
        lcd_clear(line);
    }
    hd44780_gotoxy(&lcd, 0, 0);
}

void lcd_write(const char *text, uint8_t x, uint8_t y, bool clear) {
    if (clear) {lcd_clear(y);}
    hd44780_gotoxy(&lcd, x, y);
    hd44780_puts(&lcd, text);
}

void lcd_centerwrite(const char *text, uint8_t y, bool clear) {

    int center = HD44780_COLS / 2;
    int text_length = strlen(text);

    int x = center - (text_length / 2) - 1;
    
    lcd_write(text, x, y, clear);

}

void lcd_init() {
    ESP_ERROR_CHECK(i2cdev_init());
    lcd_initialize();
    lcd_fullclear();
}
