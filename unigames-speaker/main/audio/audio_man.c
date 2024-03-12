#include "audio_man.h"

static const char *TAG = "PLAY_SDCARD_MUSIC";

audio_component_t init_audio()
{
    // Example of linking elements into an audio pipeline -- START
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t fatfs_stream_reader, i2s_stream_writer, music_decoder;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[ 1 ] Mount sdcard");
    // Initialize peripherals management
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    // Initialize SD Card peripheral
    audio_board_sdcard_init(set, SD_MODE_1_LINE);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[3.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[3.1] Create fatfs stream to read data from sdcard");
    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);

    ESP_LOGI(TAG, "[3.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[3.3] Create mp3 decoder");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    music_decoder = mp3_decoder_init(&mp3_cfg);

    ESP_LOGI(TAG, "[3.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, fatfs_stream_reader, "file");
    audio_pipeline_register(pipeline, music_decoder, "dec");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[3.5] Link it together [sdcard]-->fatfs_stream-->music_decoder-->i2s_stream-->[codec_chip]");
    const char *link_tag[3] = {"file", "dec", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "[3.6] Set up uri: /sdcard/test.mp3 ");
    audio_element_set_uri(fatfs_stream_reader, "/sdcard/TIMMERCLUB.mp3");

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    audio_component_t player = {
        .pipeline = pipeline,
        .i2s_stream_writer = i2s_stream_writer,
        .mp3_decoder = music_decoder,
        .fatfs_stream_reader = fatfs_stream_reader,
        .rsp_handle = NULL,
        .evt = evt,
        .audio_board = board_handle,
        .volume = 85,
    }; 

    return player; 
}

void play_audio(audio_component_t *player, char* uri) {
    audio_element_set_uri(player->fatfs_stream_reader, uri);
    audio_pipeline_run(player->pipeline);
}

void set_volume(audio_component_t *player, int volume) {
    player->volume = volume;
    audio_hal_set_volume(player->audio_board->audio_hal, volume);
}