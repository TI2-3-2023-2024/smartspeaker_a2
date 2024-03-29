#include "user_interface.h"
#include "../audio/audio_man.h"
#include "../audio/player.h"

#define MAX_MENU_KEY 6
#define MAX_LCD_LINES 4

/* Create ids for every menu*/
#define MENU_MAIN_0_ID 0 // sprekende klok
#define MENU_MAIN_1_ID 2 // speel unigames
#define MENU_MAIN_2_ID 6 // instellingen
#define MENU_MAIN_3_ID 17 // internet radio

#define MENU_MAIN_0_0_ID 1 // sprekende klok

#define MENU_SUB_1_0_ID 3 // talking bas
#define MENU_SUB_1_1_ID 4 // russian roulette
#define MENU_SUB_1_2_ID 5 // coin flip

#define MENU_SUB_2_0_ID 7 // instellingen beschrijving


#define MENU_SUB_1_0_0_ID 8 // Bas wacht op een vraag
#define MENU_SUB_1_0_1_ID 9 // Bas denkt na...
#define MENU_SUB_1_0_2_ID 10 // Bas geeft antwoord

#define MENU_SUB_2_1_ID 11 // sprekende klok volume
#define MENU_SUB_2_1_0_ID 12 // volume sprekende klok instellen

#define MENU_SUB_2_2_ID 13 // taal instellen
#define MENU_SUB_2_0_0_ID 14 // English
#define MENU_SUB_2_0_1_ID 15 // Nederlands
#define MENU_SUB_2_0_2_ID 16 // Français

#define MENU_SUB_3_0_1_ID 18 // internetradio afspelen

#define REC_BUTTON_ID 1
#define SET_BUTTON_ID 2
#define PLAY_BUTTON_ID 3
#define MODE_BUTTON_ID 4
#define REC_BUTTON_LONG_PRESSED_ID 7


#define LANGAUAGE_DEFAULT 0
#define LANGUAGE_DUTCH 0
#define LANGUAGE_ENGLISH 1
#define LANGUAGE_FRENCH 2

int language = LANGAUAGE_DEFAULT;

#define MAX_FILES 5
static const char *TAG = "INTERFACE";
bool mic_initialized = true;

#define DETECTION_TIMEOUT_MS 4000 // Timeout for detection in [ms]
bool introtimerended = false;
TimerHandle_t intro_timer;

typedef struct menu_item {
    unsigned int id;
    unsigned int new_id[MAX_MENU_KEY];
    char* text[MAX_LCD_LINES];
    void (*function)(int key);
} menu_item_t;

char buffer[20];

void handle_language(int key);
void set_language();
void increase_volume(int key);

//Array with directions for the interface
menu_item_t menu[] = {
    {
        MENU_MAIN_0_ID,
        /*Naliggende schermen (up, down, enter, back)*/
        {MENU_MAIN_0_ID, MENU_MAIN_1_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
        {"===HOOFDMENU===", "Sprekende Klok", "", ""},
        NULL
    },
        {
            MENU_MAIN_0_0_ID,
            {MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
            {"===Sprekende Klok===", "Tijd wordt verteld...", "", ""},
            NULL
        },
    {
        //Main screen
        MENU_MAIN_1_ID,
        {MENU_MAIN_0_ID, MENU_MAIN_2_ID, MENU_SUB_1_0_ID, MENU_MAIN_1_ID},
        {"===HOOFDMENU===", "Speel Unigames", "", ""},
        NULL
    },
        {
            //Sub screens for 1
            MENU_SUB_1_0_ID,
            {MENU_SUB_1_0_ID, MENU_SUB_1_1_ID, MENU_SUB_1_0_0_ID, MENU_MAIN_1_ID}, //edit enter
            {"===UNIGAMES===", "Talking Bas", "", ""},
            NULL
        },
        {
            //Sub screens for 1
            MENU_SUB_1_1_ID,
            {MENU_SUB_1_0_ID, MENU_SUB_1_2_ID, MENU_SUB_1_1_ID, MENU_MAIN_1_ID},
            {"===UNIGAMES===", "Russian Roulette", "", ""},
            NULL
        },
        {
            //Sub screens for 1
            MENU_SUB_1_2_ID,
            {MENU_SUB_1_1_ID, MENU_SUB_1_2_ID, MENU_SUB_1_2_ID, MENU_MAIN_1_ID},
            {"===UNIGAMES===", "Coin Flip", "", ""},
            NULL
        },
    {
        //Main screen
        MENU_MAIN_2_ID,
        {MENU_MAIN_1_ID, MENU_MAIN_3_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
        {"===HOOFDMENU===", "Instellingen", "", ""},
        NULL
    },
        {
            //Sub screens for 2
            MENU_SUB_2_0_ID,
            {MENU_SUB_2_0_ID, MENU_SUB_2_1_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
            {"===INSTELLINGEN===", "Hier kan je de", "instellingen van de", " klok aanpassen."},
            NULL
        },
        {
            //Sub screen for 1_0
            MENU_SUB_1_0_0_ID,
            {MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_ID},
            {"===TALKING BAS===", "Bas wacht op een", "vraag...", ""},
            NULL
        },
        {
            //Sub screen for 1_0_0
            MENU_SUB_1_0_1_ID,
            {MENU_SUB_1_0_1_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_2_ID, MENU_SUB_1_0_1_ID},
            {"===TALKING BAS===", "Bas denkt na...", "", ""},
            NULL
        },
        {
            //Sub screen for 1_0_1
            MENU_SUB_1_0_2_ID,
            {MENU_SUB_1_0_2_ID, MENU_SUB_1_0_2_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID},
            {"===TALKING BAS===", "Bas geeft antwoord", "", ""},
            NULL
        },
        {
            //Sub screen for 2_1
            MENU_SUB_2_1_ID,
            {MENU_SUB_2_0_ID, MENU_SUB_2_2_ID, MENU_SUB_2_1_0_ID, MENU_MAIN_2_ID},
            {"===INSTELLINGEN===", "Sprekende klok", "volume instellen", ""},
            NULL
        },
        {
            //Sub screen for 2_1_0
            MENU_SUB_2_1_0_ID,
            {MENU_SUB_2_1_0_ID, MENU_SUB_2_1_0_ID, MENU_SUB_2_1_0_ID, MENU_SUB_2_1_ID},
            {"===INSTELLINGEN===", "Volume spr. klok", "", ""},
            increase_volume
        },
        {
            //Sub screen for 2_2_0
            MENU_SUB_2_2_ID,
            {MENU_SUB_2_1_ID, MENU_SUB_2_2_ID, MENU_SUB_2_0_0_ID, MENU_MAIN_2_ID},
            {"===INSTELLINGEN===", "Taal instellen", "", ""},
            NULL
        },
        {
            //Sub screen for 2_0_0
            MENU_SUB_2_0_0_ID,
            {MENU_SUB_2_0_0_ID, MENU_SUB_2_0_1_ID, MENU_SUB_2_0_0_ID, MENU_SUB_2_2_ID},
            {"===INSTELLINGEN===", "Nederlands", "", ""},
            handle_language
        },
        {
            //Sub screen for 2_0_1
            MENU_SUB_2_0_1_ID,
            {MENU_SUB_2_0_0_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_0_1_ID, MENU_SUB_2_2_ID},
            {"===INSTELLINGEN===", "English", "", ""},
            handle_language
        },
        {
            //Sub screen for 2_0_2
            MENU_SUB_2_0_2_ID,
            {MENU_SUB_2_0_1_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_2_ID},
            {"===INSTELLINGEN===", "Français", "", ""},
            handle_language
        },
        {
        //internet radio
        MENU_MAIN_3_ID,
            {MENU_MAIN_2_ID, MENU_MAIN_3_ID, MENU_SUB_3_0_1_ID, MENU_MAIN_3_ID},
        {"===HOOFDMENU===", "Internet Radio", "", ""},
        NULL
        },
        {
            //Sub screen for 3_0_1
            MENU_SUB_3_0_1_ID,
            {MENU_SUB_3_0_1_ID, MENU_SUB_3_0_1_ID, MENU_SUB_3_0_1_ID, MENU_MAIN_3_ID},
            {"===INTERNET RADIO===", "u luistert nu naar", "", ""},
            NULL
        }
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

void introtimer_callbacked(TimerHandle_t xTimer)
{
    ESP_LOGE(TAG, "Mic initialized");
    mic_init(talking_bas_random);
}


void start_intro_timer()
{
    if (intro_timer == NULL)
    {
        intro_timer = xTimerCreate("introTimer", pdMS_TO_TICKS(4000), pdFALSE, (void *)0, introtimer_callbacked);
    }

    if (intro_timer != NULL)
    {
        xTimerStart(intro_timer, 0);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create intro timer");
    }
    introtimerended = false;
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
            play_audio(&player, "/sdcard/intro/welkomb.mp3");
            start_intro_timer();
        }
        if (current_menu_id == MENU_SUB_3_0_1_ID) {
            play_audio(&player, "/sdcard/intro/welkomi.mp3");
            start_intro_timer();
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

    if (menu[current_menu_index].function != NULL) {
        menu[current_menu_index].function(key);
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

void increase_volume(int key) {

    switch (key) {
    case REC_BUTTON_ID:
        player.volume += 10;
        break;

    case MODE_BUTTON_ID:
        player.volume -= 10;
        break;
    
    default:
        break;
    }

    if (player.volume > 100) {
        player.volume = 100;
    }
    else if (player.volume < 0) {
        player.volume = 0;
    }

    sprintf(buffer, "Volume: %d", player.volume);
    lcd_clear(3);
    lcd_write(buffer, 0, 3, false);
}

void handle_language(int key) {
    switch (key)
    {
    case PLAY_BUTTON_ID:
        set_language();
        break;
    
    default:
        break;
    }
}

void set_language() {
    switch (current_menu_index) {

    case MENU_SUB_2_0_0_ID:
        language = LANGUAGE_DUTCH;
        ESP_LOGE("Language", "Language set to Dutch");
        lcd_clear(2);
        lcd_clear(3);
        lcd_write("Taal ingesteld op", 0, 2, false);
        lcd_write("Nederlands", 0, 3, false);
        break;

    case MENU_SUB_2_0_1_ID:
        language = LANGUAGE_ENGLISH;
        ESP_LOGE("Language", "Language set to English");
        lcd_clear(2);
        lcd_clear(3);
        lcd_write("Language set to", 0, 2, false);
        lcd_write("English", 0, 3, false);
        break;

    case MENU_SUB_2_0_2_ID:
        language = LANGUAGE_FRENCH;
        ESP_LOGE("Language", "Language set to French");
        lcd_clear(2);
        lcd_clear(3);
        lcd_write("Langue réglée sur", 0, 2, false);
        lcd_write("Français", 0, 3, false);
        break;
    
    default:
        break;
    }
}
