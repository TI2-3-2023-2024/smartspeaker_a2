#ifndef SD_READER_H
#define SD_READER_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "filter_resample.h"

#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "periph_touch.h"
#include "periph_button.h"
#include "input_key_service.h"
#include "periph_adc_button.h"
#include "board.h"

#include "sdcard_list.h"
#include "sdcard_scan.h"

// Custom file marker structure
typedef struct {
    const uint8_t *start;
    const uint8_t *end;
} file_marker_t;

// Custom audio component structure
typedef struct {
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_writer;
    audio_element_handle_t mp3_decoder;
    audio_event_iface_handle_t evt;
    audio_board_handle_t audio_board;
    int volume;
} audio_component_t;

// Function prototypes
int mp3_music_read_cb(audio_element_handle_t el, char *buf, int len, TickType_t wait_time, void *ctx);
audio_component_t init_audio(i2s_stream_cfg_t i2s_cfg);
void set_file_marker(file_marker_t *marker);
void play_audio(audio_component_t *player, file_marker_t *marker);
void play_audio_with_event(audio_component_t *player, file_marker_t *marker, void(*on_finished)());
void stop_audio(audio_component_t *player);
void pause_audio(audio_component_t *player);
void resume_audio(audio_component_t *player);
void set_volume(audio_component_t *player, int volume);

#endif  

