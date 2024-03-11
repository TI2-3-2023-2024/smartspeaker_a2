#include "audio_http.h"
#include "http_stream.h"

#define TAG "AUDIO_HTTP"

/**
 * @brief Event handler for HTTP stream events.
 * 
 * @param msg HTTP stream event message.
 * @return ESP_OK on success, otherwise an error code.
 */
static int http_stream_event_handle(http_stream_event_msg_t *msg) {
    // Handle HTTP stream events if needed
    return ESP_OK;
}

/**
 * @brief Initialize the HTTP audio streaming component.
 * 
 * @return audio element handle for the HTTP audio streaming component.
 */
audio_element_handle_t audio_http_init(void) {
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = http_stream_event_handle;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    audio_element_handle_t http_stream_reader = http_stream_init(&http_cfg);
    return http_stream_reader;
}
