#include "user_interface.h"
#include "../audio/audio_man.h"
#include "../audio/player.h"

#define MAX_MENU_KEY 6
#define MAX_LCD_LINES 4

/* Create ids for every menu*/
#define MENU_MAIN_0_ID 0 // sprekende klok
#define MENU_MAIN_1_ID 2 // speel unigames
#define MENU_MAIN_2_ID 6 // instellingen

#define MENU_MAIN_0_0_ID 1 // sprekende klok
#define MENU_SUB_1_0_ID 3 // talking bas
#define MENU_SUB_1_1_ID 4 // russian roulette
#define MENU_SUB_1_2_ID 5 // coin flip
#define MENU_SUB_2_0_ID 7 // instellingen beschrijvinga
#define MENU_SUB_1_0_0_ID 8 // Bas wacht op een vraag
#define MENU_SUB_1_0_1_ID 9 // Bas denkt na...
#define MENU_SUB_1_0_2_ID 10 // Bas geeft antwoord

#define REC_BUTTON_ID 1
#define SET_BUTTON_ID 2
#define PLAY_BUTTON_ID 3
#define MODE_BUTTON_ID 4
#define REC_BUTTON_LONG_PRESSED_ID 7

#define MAX_FILES 5

static const char *TAG = "INTERFACE";

bool mic_initialized = true;

typedef struct menu_item {
    unsigned int id;
    unsigned int new_id[MAX_MENU_KEY];
    char* text[MAX_LCD_LINES];
} menu_item_t;

//Array with directions for the interface
menu_item_t menu[] = {
    {
        MENU_MAIN_0_ID,
        /*Naliggende schermen (up, down, enter, back)*/
        {MENU_MAIN_0_ID, MENU_MAIN_1_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
        {"===HOOFDMENU===", "Sprekende Klok", "", ""}
    },
        {
            MENU_MAIN_0_0_ID,
            {MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
            {"===Sprekende Klok===", "Tijd wordt verteld...", "", ""}
        },
    {
        //Main screen
        MENU_MAIN_1_ID,
        {MENU_MAIN_0_ID, MENU_MAIN_2_ID, MENU_SUB_1_0_ID, MENU_MAIN_1_ID},
        {"===HOOFDMENU===", "Speel Unigames", "", ""}
    },
        {
            //Sub screens for 1
            MENU_SUB_1_0_ID,
            {MENU_SUB_1_0_ID, MENU_SUB_1_1_ID, MENU_SUB_1_0_0_ID, MENU_MAIN_1_ID}, //edit enter
            {"===UNIGAMES===", "Talking Bas", "", ""}
        },
        {
            //Sub screens for 1
            MENU_SUB_1_1_ID,
            {MENU_SUB_1_0_ID, MENU_SUB_1_2_ID, MENU_SUB_1_1_ID, MENU_MAIN_1_ID},
            {"===UNIGAMES===", "Russian Roulette", "", ""}
        },
        {
            //Sub screens for 1
            MENU_SUB_1_2_ID,
            {MENU_SUB_1_1_ID, MENU_SUB_1_2_ID, MENU_SUB_1_2_ID, MENU_MAIN_1_ID},
            {"===UNIGAMES===", "Coin Flip", "", ""}
        },
    {
        //Main screen
        MENU_MAIN_2_ID,
        {MENU_MAIN_1_ID, MENU_MAIN_2_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
        {"===HOOFDMENU===", "Instellingen", "", ""}
    },
        {
            //Sub screens for 2
            MENU_SUB_2_0_ID,
            {MENU_SUB_2_0_ID, MENU_SUB_2_0_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
            {"===INSTELLINGEN===", "Hier kan je de", "instellingen van de", " klok aanpassen."}
        },
        {
            //Sub screen for 1_0
            MENU_SUB_1_0_0_ID,
            {MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_ID},
            {"===TALKING BAS===", "Bas wacht op een", "vraag...", ""}
        },
        {
            //Sub screen for 1_0_0
            MENU_SUB_1_0_1_ID,
            {MENU_SUB_1_0_1_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_2_ID, MENU_SUB_1_0_1_ID},
            {"===TALKING BAS===", "Bas denkt na...", "", ""}
        },
        {
            //Sub screen for 1_0_1
            MENU_SUB_1_0_2_ID,
            {MENU_SUB_1_0_2_ID, MENU_SUB_1_0_2_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID},
            {"===TALKING BAS===", "Bas geeft antwoord", "", ""}
        },
};

static unsigned int current_menu_index = MENU_MAIN_0_ID;
static unsigned int current_menu_id;

void talking_bas_random() {
    ESP_LOGE(TAG, "Talking Bas");
    
}

void print_menu_item(char** text[]);
void clear_menu();
void write_time();

void menu_start() {
    print_menu_item(menu[current_menu_index].text);
}

void handle_menu(int key) {

    switch (key) {
    //up
    case REC_BUTTON_ID:
        current_menu_id = menu[current_menu_index].new_id[0];
        break;
    //back
    case SET_BUTTON_ID:
        current_menu_id = menu[current_menu_index].new_id[3];

        if (current_menu_id == MENU_SUB_1_0_ID)
        {
           ESP_LOGE(TAG, "Mic stopped");
           mic_stop();
        }
        break;
    //enter
    case PLAY_BUTTON_ID:
        current_menu_id = menu[current_menu_index].new_id[2];
        if (current_menu_id == MENU_SUB_1_0_0_ID) {
            ESP_LOGE(TAG, "Mic initialized");
            mic_init(talking_bas_random);   
        }
        break;
    
    //down
    case MODE_BUTTON_ID:
        current_menu_id = menu[current_menu_index].new_id[1];
        break;

    case REC_BUTTON_LONG_PRESSED_ID:
        write_time();
        tell_time(get_time());
        return;
        break;
    
    default:
        break;
    }

    if (current_menu_id != current_menu_index) {
        current_menu_index = current_menu_id;
        print_menu_item(menu[current_menu_index].text);
    }
}

void write_time() {
    clear_menu();
    struct DateTime dt = get_time();

    lcd_centerwrite("Tijd:", 1, false);

    char buffer[20];
    sprintf(buffer, "%02d:%02d", dt.hour, dt.minute);
    lcd_centerwrite(buffer, 2, false);
}

void print_menu_item(char** text[]) {
   
    clear_menu();
    for (int i = 0; i < MAX_LCD_LINES; i++) {
        if (text[i] != NULL) {
            lcd_write(text[i], 0, i, false);
        }
    }
}

void clear_menu() {
    for (size_t i = 0; i < MAX_LCD_LINES; i++) {
        lcd_clear(i);
    }
}