#include "sd_reader.h"

static const char *TAG = "PLAY_FLASH_MP3_CONTROL";

static struct {
    int pos;
    const uint8_t *start;
    const uint8_t *end;
} file_marker;

int mp3_music_read_cb(audio_element_handle_t el, char *buf, int len, TickType_t wait_time, void *ctx) {
    int read_size = file_marker.end - file_marker.start - file_marker.pos;
    if (read_size == 0) {
        return AEL_IO_DONE;
    } else if (len < read_size) {
        read_size = len;
    }
    memcpy(buf, file_marker.start + file_marker.pos, read_size);
    file_marker.pos += read_size;
    return read_size;
}

audio_component_t init_audio(i2s_stream_cfg_t i2s_cfg) {
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_writer, mp3_decoder;
    audio_event_iface_handle_t evt;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    int player_volume;
    audio_hal_get_volume(board_handle->audio_hal, &player_volume);

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);
    audio_element_set_read_cb(mp3_decoder, mp3_music_read_cb, NULL);

    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    const char *link_tag[2] = {"mp3", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 2);

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    audio_board_key_init(set);

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    audio_pipeline_set_listener(pipeline, evt);

    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    audio_component_t player = {
        .pipeline = pipeline,
        .i2s_stream_writer = i2s_stream_writer,
        .mp3_decoder = mp3_decoder,
        .evt = evt,
        .audio_board = board_handle,
        .volume = player_volume
    };

    return player;
}

void set_file_marker(file_marker_t *marker) {
    file_marker.start = marker->start;
    file_marker.end = marker->end;
    file_marker.pos = 0;
}

void play_audio(audio_component_t *player, file_marker_t *marker) {
    set_file_marker(marker);
    audio_pipeline_run(player->pipeline);
}

void play_audio_with_event(audio_component_t *player, file_marker_t *marker, void(*on_finished)()) {
    audio_element_handle_t mp3_decoder = player->mp3_decoder;
    audio_element_handle_t i2s_stream_writer = player->i2s_stream_writer;
    set_file_marker(marker);
    audio_pipeline_run(player->pipeline);
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(player->evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) mp3_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(mp3_decoder, &music_info);
            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
            continue;
        }

        audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
        switch (el_state) {
            case AEL_STATE_INITIALIZING:
                // Initialization code
                break;
            case AEL_STATE_FINISHED:
                if (on_finished != NULL) {
                    // blub
                    on_finished();
                }
                break;
            default:
                break;
        }
    }
}

void stop_audio(audio_component_t *player) {
    audio_pipeline_stop(player->pipeline);
}

void pause_audio(audio_component_t *player) {
    audio_pipeline_pause(player->pipeline);
}

void resume_audio(audio_component_t *player) {
    audio_pipeline_resume(player->pipeline);
}

void set_volume(audio_component_t *player, int volume) {
    player->volume = volume;
    audio_hal_set_volume(player->audio_board->audio_hal, player->volume);
}
