#include "user_interface.h"

#define MAX_MENU_KEY 6
#define MAX_LCD_LINES 4

/* Create ids for every */
#define MENU_MAIN_0_ID 0 // sprekende klok
#define MENU_MAIN_1_ID 1 // speel unigames
#define MENU_MAIN_2_ID 2 // instellingen

#define MENU_MAIN_0_0_ID 3 // sprekende klok
#define MENU_SUB_1_0_ID 4 // talking bas
#define MENU_SUB_1_1_ID 5 // russian roulette
#define MENU_SUB_1_2_ID 6 // coin flip


typedef struct menu_item {
    unsigned int id;
    unsigned int new_id[MAX_MENU_KEY];
    char* text[MAX_LCD_LINES];
} menu_item_t;

menu_item_t menu[] {
    {
        MENU_MAIN_0_ID,
        /*Naliggende schermen (up, down, enter, back)*/
    }


}
