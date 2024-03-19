#ifndef MIC_MAN_H
#define MIC_MAN_H

#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_log.h"
#include "board.h"
#include "audio_common.h"
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "raw_stream.h"
#include "filter_resample.h"

#include "goertzel_filter.h"

void mic_init(void (*callback)());
void mic_stop(void);


#endif // MIC_MAN_H