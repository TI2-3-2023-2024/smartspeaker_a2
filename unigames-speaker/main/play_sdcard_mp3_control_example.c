#include "sd_reader.h"


void app_main(void) {

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    
    audio_player_init();
    audio_player_play();
}


