#include "audio_man.h"

static const char *TAG = "audio_man";

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

/// @brief the audio pipeline is initialized and the audio components are registered, always use this function
/// to initialize the audio pipeline
/// @return a handle to the audio pipeline
audio_component_t init_audio() {
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_writer, mp3_decoder, fatfs_stream_reader, rsp_handle;

    ESP_LOGI(TAG, "[1.0] Initialize peripherals management");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[1.1] Initialize and start peripherals");
    audio_board_key_init(set);
    audio_board_sdcard_init(set, SD_MODE_1_LINE);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[4.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[4.1] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    i2s_stream_set_clk(i2s_stream_writer, 48000, 16, 2);

    ESP_LOGI(TAG, "[4.2] Create mp3 decoder to decode mp3 file");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);

    /* ZL38063 audio chip on board of ESP32-LyraTD-MSC does not support 44.1 kHz sampling frequency,
       so resample filter has been added to convert audio data to other rates accepted by the chip.
       You can resample the data to 16 kHz or 48 kHz.
    */
    ESP_LOGI(TAG, "[4.3] Create resample filter");
    rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rsp_handle = rsp_filter_init(&rsp_cfg);

    ESP_LOGI(TAG, "[4.4] Create fatfs stream to read data from sdcard");
    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);

    ESP_LOGI(TAG, "[4.5] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, fatfs_stream_reader, "file");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, rsp_handle, "filter");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[4.6] Link it together [sdcard]-->fatfs_stream-->mp3_decoder-->resample-->i2s_stream-->[codec_chip]");
    const char *link_tag[4] = {"file", "mp3", "filter", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 4);

    ESP_LOGI(TAG, "[5.0] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[5.1] Listen for all pipeline events");
    audio_pipeline_set_listener(pipeline, evt);
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    int player_volume = 80;

    audio_component_t player = {
        .pipeline = pipeline,
        .i2s_stream_writer = i2s_stream_writer,
        .mp3_decoder = mp3_decoder,
        .fatfs_stream_reader = fatfs_stream_reader,
        .rsp_handle = NULL,
        .evt = evt,
        .audio_board = board_handle,
        .rsp_handle = rsp_handle,
        .fatfs_stream_reader = fatfs_stream_reader,
        .volume = player_volume
    };

    return player; 
}

/// @brief the audio pipeline is started and the audio is played
/// @param player the player whose audio pipeline is to be started
/// @param url the url of the audio file to be played from the sdcard
void play_audio(audio_component_t *player, char* url) {
    audio_hal_set_volume(player->audio_board->audio_hal, player->volume);
    audio_element_set_uri(player->fatfs_stream_reader, url);
    audio_pipeline_reset_ringbuffer(player->pipeline);
    audio_pipeline_reset_elements(player->pipeline);
    audio_pipeline_change_state(player->pipeline, AEL_STATE_INIT);
    audio_pipeline_run(player->pipeline);
}

/// @brief stops the audio pipeline
/// @param player the player whose audio pipeline is to be stopped
void stop_audio(audio_component_t *player) {
    audio_pipeline_stop(player->pipeline);
}

/// @brief pauses the audio pipeline
/// @param player the player whose audio pipeline is to be paused
void pause_audio(audio_component_t *player) {
    audio_pipeline_pause(player->pipeline);
}

/// @brief resumes the audio pipeline
/// @param player player whose audio pipeline is to be resumed
void resume_audio(audio_component_t *player) {
    audio_pipeline_resume(player->pipeline);
}

/// @brief sets the volume of the audio pipeline
/// @param player the player whose audio pipeline is to be adjusted
/// @param volume the volume to be set
void set_volume(audio_component_t *player, int volume) {
    player->volume = volume;
    audio_hal_set_volume(player->audio_board->audio_hal, player->volume);
}

/// @brief  plays multiple audio files from the sdcard
/// @param pvParameters the playlist of audio files
void play_multiple_audio_task(void *pvParameters) {
    playlist_t *playlist = (playlist_t *) pvParameters;
    int playlist_index = 0;

    audio_hal_set_volume(playlist->player.audio_board->audio_hal, playlist->player.volume);

    audio_element_set_uri(playlist->player.fatfs_stream_reader, playlist->file_uris[0]);
    audio_pipeline_reset_ringbuffer(playlist->player.pipeline);
    audio_pipeline_reset_elements(playlist->player.pipeline);
    audio_pipeline_change_state(playlist->player.pipeline, AEL_STATE_INIT);
    audio_pipeline_run(playlist->player.pipeline);

    while (1) {
        /* Handle event interface messages from pipeline
           to set music info and to advance to the next song
        */
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(playlist->player.evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT) {
            // Set music info for a new song to be played
            if (msg.source == (void *) playlist->player.mp3_decoder
                && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
                audio_element_info_t music_info = {0};
                audio_element_getinfo(playlist->player.mp3_decoder, &music_info);
                ESP_LOGI(TAG, "[ * ] Received music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
                         music_info.sample_rates, music_info.bits, music_info.channels);
                audio_element_setinfo(playlist->player.i2s_stream_writer, &music_info);
                rsp_filter_set_src_info(playlist->player.rsp_handle, music_info.sample_rates, music_info.channels);
                continue;
            }
            // Advance to the next song when previous finishes
            if (msg.source == (void *) playlist->player.i2s_stream_writer
                && msg.cmd == AEL_MSG_CMD_REPORT_STATUS) {
                audio_element_state_t el_state = audio_element_get_state(playlist->player.i2s_stream_writer);
                if (el_state == AEL_STATE_FINISHED) {
                    ESP_LOGI(TAG, "[ * ] Finished, advancing to the next song");
                    playlist_index++;

                    if (playlist_index >= playlist->number_of_files) {
                        if (playlist->on_finished != NULL) {
                            playlist->on_finished();
                        }
                        vTaskDelete(NULL);
                        break;
                    }

                    ESP_LOGW(TAG, "URL: %s", playlist->file_uris[playlist_index]);
                    /* In previous versions, audio_pipeline_terminal() was called here. It will close all the element task and when we use
                     * the pipeline next time, all the tasks should be restarted again. It wastes too much time when we switch to another music.
                     * So we use another method to achieve this as below.
                     */
                    audio_element_set_uri(playlist->player.fatfs_stream_reader, playlist->file_uris[playlist_index]);
                    audio_pipeline_reset_ringbuffer(playlist->player.pipeline);
                    audio_pipeline_reset_elements(playlist->player.pipeline);
                    audio_pipeline_change_state(playlist->player.pipeline, AEL_STATE_INIT);
                    audio_pipeline_run(playlist->player.pipeline);
                }
                continue;
            }
        }
    }
}

