#ifndef CLOCK_MAN_H
#define CLOCK_MAN_H

#include "../time/datetime.h"
#include "../audio/audio_man.h"

void wait(unsigned int seconds);
void tell_time(struct DateTime dt, audio_component_t player);
void display_time(audio_component_t player);

#endif
