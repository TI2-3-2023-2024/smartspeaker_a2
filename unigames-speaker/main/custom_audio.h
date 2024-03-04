#ifndef CUSTOM_AUDIO_H
#define CUSTOM_AUDIO_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_mem.h"
#include "audio_common.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"

/// @brief Structure to hold the start and end of a file
typedef struct {
    const uint8_t *start;
    const uint8_t *end;
} file_marker_t;

/// @brief Structure to hold the audio pipeline and its components, also contains the volume
typedef struct audio_component {
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_writer, mp3_decoder;
    audio_event_iface_handle_t evt;
    audio_board_handle_t audio_board;
    int volume;
} audio_component_t;

void set_file_marker(file_marker_t *marker);
audio_component_t init_audio(i2s_stream_cfg_t i2s_cfg);
void play_audio(audio_component_t *player, file_marker_t *marker);
void play_audio_with_event(audio_component_t *player, file_marker_t *marker, void(*on_finshed)());
void stop_audio(audio_component_t *player);
void set_volume(audio_component_t *player, int volume);
void pause_audio(audio_component_t *player);

#endif