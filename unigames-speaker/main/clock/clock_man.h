#ifndef CLOCK_MAN_H
#define CLOCK_MAN_H

#include "../time/datetime.h"
#include "../audio/audio_man.h"

void wait(unsigned int seconds);
void tell_time(struct DateTime dt);
void display_time(void);

#endif
