#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


#include <stdio.h>
#include "esp_log.h"
#include "../lcd/lcd_man.h"
#include "../time/time_man.h"
#include "../clock/clock_man.h"
#include "../audio/player.h"

void menu_start();
void handle_menu(int key);

#endif