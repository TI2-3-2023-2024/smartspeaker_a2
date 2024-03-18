#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


#include <stdio.h>
#include "../lcd/lcd_man.h"
#include "../time/time_man.h"
#include "../clock/clock_man.h"
#include "../audio/player.h"
#include "../microphone/mic_man.h"

void menu_start();
void handle_menu(int key);

#endif