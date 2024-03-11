#ifndef AUDIO_HTTP_H
#define AUDIO_HTTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_element.h"

/**
 * @brief Initialize the HTTP audio streaming component.
 * 
 * @return audio element handle for the HTTP audio streaming component.
 */
audio_element_handle_t audio_http_init(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_HTTP_H