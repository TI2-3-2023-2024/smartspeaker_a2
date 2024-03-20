#include "mic_man.h"
#include "../audio/player.h"
#include "../audio/audio_man.h"
#include "../interface/language.h"

#define MAX_FILES 5

static const char *TAG = "TALKING_BAS_MICROPHONE";

QueueHandle_t button_queue;

bool button_pressed = false;

void (*function_callback)(void);

audio_pipeline_handle_t pipeline;

audio_element_handle_t i2s_stream_reader;
audio_element_handle_t resample_filter;
audio_element_handle_t raw_reader;

#define GOERTZEL_SAMPLE_RATE_HZ 8000 // Sample rate in [Hz]
#define GOERTZEL_FRAME_LENGTH_MS 100 // Block length in [ms]

#define GOERTZEL_BUFFER_LENGTH (GOERTZEL_FRAME_LENGTH_MS * GOERTZEL_SAMPLE_RATE_HZ / 1000) // Buffer length in samples

#define GOERTZEL_DETECTION_THRESHOLD 50.0f // Detect a tone when log magnitude is above this value

#define AUDIO_SAMPLE_RATE 48000 // Audio capture sample rate [Hz]

#define DETECTION_TIMEOUT_MS 2000 // Timeout for detection in [ms]

bool timerended = false;

static const int GOERTZEL_DETECT_FREQS[] = {
    300,
    500,
    700};

#define GOERTZEL_NR_FREQS ((sizeof GOERTZEL_DETECT_FREQS) / (sizeof GOERTZEL_DETECT_FREQS[0]))

static audio_element_handle_t create_i2s_stream(int sample_rate, audio_stream_type_t type)
{
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = type;
    i2s_cfg.i2s_config.sample_rate = sample_rate;
    audio_element_handle_t i2s_stream = i2s_stream_init(&i2s_cfg);
    return i2s_stream;
}

static audio_element_handle_t create_resample_filter(
    int source_rate, int source_channels, int dest_rate, int dest_channels)
{
    rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rsp_cfg.src_rate = source_rate;
    rsp_cfg.src_ch = source_channels;
    rsp_cfg.dest_rate = dest_rate;
    rsp_cfg.dest_ch = dest_channels;
    audio_element_handle_t filter = rsp_filter_init(&rsp_cfg);
    return filter;
}

static audio_element_handle_t create_raw_stream()
{
    raw_stream_cfg_t raw_cfg = {
        .out_rb_size = 8 * 1024,
        .type = AUDIO_STREAM_READER,
    };
    audio_element_handle_t raw_reader = raw_stream_init(&raw_cfg);
    return raw_reader;
}

static audio_pipeline_handle_t create_pipeline()
{
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    audio_pipeline_handle_t pipeline = audio_pipeline_init(&pipeline_cfg);
    return pipeline;
}

TimerHandle_t detection_timer;

char *bas_file_uris[MAX_FILES];

void timer_callbacked(TimerHandle_t xTimer)
{
    // This function will be called when the timer expires
    ESP_LOGI(TAG, "Timer elapsed");
    timerended = true;

    int random = rand() % 5;
    play_audio(&player, bas_file_uris[random]);
}

// Function to start the detection timeout timer
void start_detection_timer()
{
    if (detection_timer == NULL)
    {
        detection_timer = xTimerCreate("DetectionTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, timer_callbacked);
    }

    if (detection_timer != NULL)
    {
        xTimerStart(detection_timer, 0);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create detection timer");
    }
    timerended = false;
}

// Function to stop the detection timeout timer
void stop_detection_timer()
{
    if (detection_timer != NULL)
    {
        xTimerStop(detection_timer, 0);
        xTimerDelete(detection_timer, 0);
        detection_timer = NULL;
    }
}

/**
 * Determine if a frequency was detected or not, based on the magnitude that the
 * Goertzel filter calculated
 * Use a logarithm for the magnitude
 */
static void detect_freq(int target_freq, float magnitude)
{
    float logMagnitude = 10.0f * log10f(magnitude);

    if (logMagnitude > GOERTZEL_DETECTION_THRESHOLD)
    {
        ESP_LOGI(TAG, "Detection at frequency %d Hz (magnitude %.2f, log magnitude %.2f)", target_freq, magnitude, logMagnitude);
        start_detection_timer(); // Start or reset the detection timer
        ESP_LOGI(TAG, "Timer started");
        timerended = true; // Set flag to prevent timer from starting again
    }
    else if (!timerended)
    {
        stop_detection_timer(); // Stop the detection timer
        ESP_LOGI(TAG, "Timer stopped");
        timerended = true; // Set flag to prevent timer from starting again
    }
}

void tone_detection_task(void *pvParameters)
{
    button_pressed = false;
    goertzel_filter_cfg_t filters_cfg[GOERTZEL_NR_FREQS];
    goertzel_filter_data_t filters_data[GOERTZEL_NR_FREQS];

    ESP_LOGI(TAG, "Create raw sample buffer");
    int16_t *raw_buffer = (int16_t *)malloc((GOERTZEL_BUFFER_LENGTH * sizeof(int16_t)));
    if (raw_buffer == NULL)
    {
        ESP_LOGE(TAG, "Memory allocation for raw sample buffer failed");
    }

    ESP_LOGI(TAG, "Setup Goertzel detection filters");
    for (int f = 0; f < GOERTZEL_NR_FREQS; f++)
    {
        filters_cfg[f].sample_rate = GOERTZEL_SAMPLE_RATE_HZ;
        filters_cfg[f].target_freq = GOERTZEL_DETECT_FREQS[f];
        filters_cfg[f].buffer_length = GOERTZEL_BUFFER_LENGTH;
        esp_err_t error = goertzel_filter_setup(&filters_data[f], &filters_cfg[f]);
        ESP_ERROR_CHECK(error);
    }

    ESP_LOGI(TAG, "Create pipeline");
    pipeline = create_pipeline();

    ESP_LOGI(TAG, "Create audio elements for pipeline");
    i2s_stream_reader = create_i2s_stream(AUDIO_SAMPLE_RATE, AUDIO_STREAM_READER);
    resample_filter = create_resample_filter(AUDIO_SAMPLE_RATE, 2, GOERTZEL_SAMPLE_RATE_HZ, 1);
    raw_reader = create_raw_stream();

    ESP_LOGI(TAG, "Register audio elements to pipeline");
    audio_pipeline_register(pipeline, i2s_stream_reader, "i2s");
    audio_pipeline_register(pipeline, resample_filter, "rsp_filter");
    audio_pipeline_register(pipeline, raw_reader, "raw");

    ESP_LOGI(TAG, "Link audio elements together to make pipeline ready");
    const char *link_tag[3] = {"i2s", "rsp_filter", "raw"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "Start pipeline");
    audio_pipeline_run(pipeline);

    while (1)
    {
        if (button_pressed)
        {
            break;
        }
        raw_stream_read(raw_reader, (char *)raw_buffer, GOERTZEL_BUFFER_LENGTH * sizeof(int16_t));
        for (int f = 0; f < GOERTZEL_NR_FREQS; f++)
        {
            float magnitude;
            if (button_pressed)
            {
                break;
            }
            esp_err_t error = goertzel_filter_process(&filters_data[f], raw_buffer, GOERTZEL_BUFFER_LENGTH);
            ESP_ERROR_CHECK(error);

            if (goertzel_filter_new_magnitude(&filters_data[f], &magnitude))
            {
                detect_freq(filters_cfg[f].target_freq, magnitude);
            }
        }
    }

    vTaskDelete(NULL);
    // Clean up (if we somehow leave the while loop, that is...)
    ESP_LOGI(TAG, "Deallocate raw sample buffer memory");
    free(raw_buffer);

    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, i2s_stream_reader);
    audio_pipeline_unregister(pipeline, resample_filter);
    audio_pipeline_unregister(pipeline, raw_reader);

    audio_pipeline_deinit(pipeline);

    audio_element_deinit(i2s_stream_reader);
    audio_element_deinit(resample_filter);
    audio_element_deinit(raw_reader);
}

void mic_init(void (*callback)())
{
    *bas_file_uris = malloc(MAX_FILES * sizeof(char *));

    // check which language is active
    if (language == 0)
    {
        bas_file_uris[0] = "/sdcard/nl/games/bas/BAS.mp3";
        bas_file_uris[1] = "/sdcard/nl/games/bas/JA.mp3";
        bas_file_uris[2] = "/sdcard/nl/games/bas/BUHHH.mp3";
        bas_file_uris[3] = "/sdcard/nl/games/bas/HOHOHO.mp3";
        bas_file_uris[4] = "/sdcard/nl/games/bas/NEE.mp3";
    }
    else if (language == 1)
    {
        bas_file_uris[0] = "/sdcard/en/games/bas/BAS2.mp3";
        bas_file_uris[1] = "/sdcard/en/games/bas/YES.mp3";
        bas_file_uris[2] = "/sdcard/en/games/bas/BUHHH2.mp3";
        bas_file_uris[3] = "/sdcard/en/games/bas/HOHOHO2.mp3";
        bas_file_uris[4] = "/sdcard/en/games/bas/NO.mp3";
    }
    else if (language == 2)
    {
        bas_file_uris[0] = "/sdcard/fr/games/bas/BAS2.mp3";
        bas_file_uris[1] = "/sdcard/fr/games/bas/OUI.mp3";
        bas_file_uris[2] = "/sdcard/fr/games/bas/BUHHH2.mp3";
        bas_file_uris[3] = "/sdcard/fr/games/bas/HOHOHO2.mp3";
        bas_file_uris[4] = "/sdcard/fr/games/bas/NON.mp3";
    }

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    // Setup audio codec
    ESP_LOGI(TAG, "Setup codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    // Perform tone detection task
    xTaskCreate(tone_detection_task, "tone_detection_task", 4096, NULL, 5, NULL);
}

void mic_stop(void)
{
    button_pressed = true;

    // Stop the audio pipeline
    ESP_LOGE(TAG, "Stopping audio pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    // Deactivate the audio codec
    ESP_LOGE(TAG, "Deactivating audio codec");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_STOP);
}
