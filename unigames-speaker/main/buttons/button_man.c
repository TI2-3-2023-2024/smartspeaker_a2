#include "esp_log.h"
#include "board.h"
#include "esp_peripherals.h"
#include "periph_adc_button.h"
#include "input_key_service.h"
#include <stdio.h>
#include "button_man.h"
#include "button_tasks.h"

static const char *TAG = "Button";

void (*on_pressed)(int) = NULL;

/// @brief Callback function for handling input key events.
/// @param handle The handle of the peripheral service.
/// @param evt Pointer to the event structure containing key event information.
/// @param ctx Pointer to the context data (unused in this function).
/// @return ESP_OK if the key event is handled successfully, else an error code.
static esp_err_t input_key_service_cb(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{

    // rec = 1
    // set = 2
    // play = 3
    // mode = 4

    void (*press_pointer)(int) = pressed;
    ESP_LOGD(TAG, "[ * ] input key id is %d, %d", (int)evt->data, evt->type);
    const char *key_types[INPUT_KEY_SERVICE_ACTION_PRESS_RELEASE + 1] = {"UNKNOWN", "ingedrukt", "snel losgelaten", "vastgehouden", "vastgehouden losgelaten"};
    switch ((int)evt->data) {
        case INPUT_KEY_USER_ID_REC:
            ESP_LOGI(TAG, "[Rec] %s", key_types[evt->type]);
            if(key_types[evt->type] == key_types[2]){
                on_pressed(1);
            } else if(key_types[evt->type] == key_types[3]){
                on_pressed(7);
            }
            break;
        case INPUT_KEY_USER_ID_SET:
            ESP_LOGI(TAG, "[Set] %s", key_types[evt->type]);
            if(key_types[evt->type] == key_types[2]){
                on_pressed(2);
            }
            break;
        case INPUT_KEY_USER_ID_PLAY:
            ESP_LOGI(TAG, "[Play] %s", key_types[evt->type]);
            if(key_types[evt->type] == key_types[2]){
                on_pressed(3);
            }
            break;
        case INPUT_KEY_USER_ID_MODE:
            ESP_LOGI(TAG, "[Mode] %s", key_types[evt->type]);
             if(key_types[evt->type] == key_types[2]){
                on_pressed(4);
             }
            break;
        case INPUT_KEY_USER_ID_VOLDOWN:
            ESP_LOGI(TAG, "[Vol-] %s", key_types[evt->type]);
            if(key_types[evt->type] == key_types[2]){
                on_pressed(5);
            }
            break;
        case INPUT_KEY_USER_ID_VOLUP:
            ESP_LOGI(TAG, "[Vol+] %s", key_types[evt->type]);
            if(key_types[evt->type] == key_types[2]){
                on_pressed(6);
                }
            break;
        default:
            press_pointer(8);
            break;
    }
    return ESP_OK;

}

/// @brief Initializes the button handler module and sets the callback function for button press events.
/// @param on_pressed_callback Pointer to the function that will handle button press events.
void button_han_init(void (*on_pressed_callback)(int))
{
    on_pressed = on_pressed_callback;
    ESP_LOGI(TAG, "[ 1 ] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[ 2 ] Initialize Button peripheral with board init");
    audio_board_key_init(set);

    ESP_LOGI(TAG, "[ 3 ] Create and start input key service");
    input_key_service_info_t input_key_info[] = INPUT_KEY_DEFAULT_INFO();
    input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
    input_cfg.handle = set;
    input_cfg.based_cfg.task_stack = 4 * 1024;
    periph_service_handle_t input_ser = input_key_service_create(&input_cfg);

    input_key_service_add_key(input_ser, input_key_info, INPUT_KEY_NUM);
    periph_service_set_callback(input_ser, input_key_service_cb, NULL);

    ESP_LOGW(TAG, "[ 4 ] Waiting for a button to be pressed ...");
}
   