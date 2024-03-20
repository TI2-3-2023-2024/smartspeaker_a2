#include "user_interface.h"
#include "../audio/audio_man.h"
#include "../audio/player.h"

#define MAX_MENU_KEY 6
#define MAX_LCD_LINES 4

/* Create ids for every menu */
#define MENU_MAIN_0_ID 0 // sprekende klok
#define MENU_MAIN_1_ID 2 // speel unigames
#define MENU_MAIN_2_ID 6 // instellingen

#define MENU_MAIN_0_0_ID 1 // sprekende klok

#define MENU_SUB_1_0_ID 3 // talking bas
#define MENU_SUB_1_1_ID 4 // russian roulette
#define MENU_SUB_1_2_ID 5 // coin flip

#define MENU_SUB_2_0_ID 7 // instellingen beschrijving

#define MENU_SUB_1_0_0_ID 8  // Bas wacht op een vraag
#define MENU_SUB_1_0_1_ID 9  // Bas denkt na...
#define MENU_SUB_1_0_2_ID 10 // Bas geeft antwoord

#define MENU_SUB_2_1_ID 11    // sprekende klok volume
#define MENU_SUB_2_1_0_ID 12 // volume sprekende klok instellen

#define MENU_SUB_2_2_ID 13 // taal instellen
#define MENU_SUB_2_0_0_ID 14 // English
#define MENU_SUB_2_0_1_ID 15 // Nederlands
#define MENU_SUB_2_0_2_ID 16 // Français

#define REC_BUTTON_ID 1
#define SET_BUTTON_ID 2
#define PLAY_BUTTON_ID 3
#define MODE_BUTTON_ID 4
#define REC_BUTTON_LONG_PRESSED_ID 7

#define LANGAUAGE_DEFAULT 0
#define LANGUAGE_DUTCH 0
#define LANGUAGE_ENGLISH 1
#define LANGUAGE_FRENCH 2

char instellingen[20] = "===INSTELLINGEN===";
char instellingen_NL[] = "===INSTELLINGEN===";
char instellingen_EN[] = "===SETTINGS===";
char instellingen_FR[] = "===RÉGLAGES===";

char hoofdmenu[20] = "===HOOFDMENU===";
char hoofdmenu_NL[] = "===HOOFDMENU===";
char hoofdmenu_EN[] = "===MAIN MENU===";
char hoofdmenu_FR[] = "===MENU PRINCIPAL===";

char sprekendeklok[20] = "===SPREKENDE KLOK===";
char sprekendeklok_NL[] = "===SPREKENDE KLOK===";
char sprekendeklok_EN[] = "===TALKING CLOCK===";
char sprekendeklok_FR[] = "===HORLOGE PARLANTE===";

char sprekendklokje[20] = "Sprekende Klok";
char sprekendklokje_NL[] = "Sprekende Klok";
char sprekendklokje_EN[] = "Talking Clock";
char sprekendklokje_FR[] = "Horloge Parlante";

char timeistold[20] = "Tijd wordt verteld...";
char timeistold_NL[] = "Tijd wordt verteld...";
char timeistold_EN[] = "Time is being told...";
char timeistold_FR[] = "L'heure est dite...";

char speelunigames[20] = "Speel Unigames";
char speelunigames_NL[] = "Speel Unigames";
char speelunigames_EN[] = "Play Unigames";
char speelunigames_FR[] = "Jouer Unigames";

char instellingen1[20] = "Instellingen";
char instellingen1_NL[] = "Instellingen";
char instellingen1_EN[] = "Settings";
char instellingen1_FR[] = "Reglages";

char hierkanjede[20] = "Hier kan je de";
char hierkanjede_NL[] = "Hier kan je de";
char hierkanjede_EN[] = "Here you can adjust the";
char hierkanjede_FR[] = "Ici vous pouvez ajuster les";

char instellingenvande[20] = "instellingen van de";
char instellingenvande_NL[] = "instellingen van de";
char instellingenvande_EN[] = "settings of the";
char instellingenvande_FR[] = "paramètres de";

char klokaanpassen[20] = "klok aanpassen.";
char klokaanpassen_NL[] = "klok aanpassen.";
char klokaanpassen_EN[] = "clock.";
char klokaanpassen_FR[] = "l'horloge.";

char baswacht[20] = "Bas wacht op een";
char baswacht_NL[] = "Bas wacht op een";
char baswacht_EN[] = "Bas is waiting for a";
char baswacht_FR[] = "Bas attend une";

char vraag[20] = "vraag...";
char vraag_NL[] = "vraag...";
char vraag_EN[] = "question...";
char vraag_FR[] = "question...";

char basdenktna[20] = "Bas denkt na...";
char basdenktna_NL[] = "Bas denkt na...";
char basdenktna_EN[] = "Bas is thinking...";
char basdenktna_FR[] = "Bas réfléchit...";

char basgeeftantwoord[20] = "Bas geeft antwoord";
char basgeeftantwoord_NL[] = "Bas geeft antwoord";
char basgeeftantwoord_EN[] = "Bas gives an answer";
char basgeeftantwoord_FR[] = "Bas donne une";

char reponse[20] = "";
char reponse_NL[] = "";
char reponse_EN[] = "";
char reponse_FR[] = "reponse";

char taalinstellen[20] = "Taal instellen";
char taalinstellen_NL[] = "Taal instellen";
char taalinstellen_EN[] = "Set language";
char taalinstellen_FR[] = "Régler la langue";

int language = LANGAUAGE_DEFAULT;

#define MAX_FILES 5
static const char *TAG = "INTERFACE";
bool mic_initialized = true;

typedef struct menu_item {
    unsigned int id;
    unsigned int new_id[MAX_MENU_KEY];
    char *text[MAX_LCD_LINES];
    void (*function)(int key);
} menu_item_t;

char buffer[20];

void handle_language(int key);
void set_language();
void increase_volume(int key);

// Array with directions for the interface
menu_item_t menu[] = {
    {
        MENU_MAIN_0_ID,
        /*Naliggende schermen (up, down, enter, back)*/
        {MENU_MAIN_0_ID, MENU_MAIN_1_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
        {hoofdmenu, sprekendklokje, "", ""},
        NULL},
    {
        MENU_MAIN_0_0_ID,
        {MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_0_ID, MENU_MAIN_0_ID},
        {sprekendeklok, timeistold, "", ""},
        NULL},
    {
        // Main screen
        MENU_MAIN_1_ID,
        {MENU_MAIN_0_ID, MENU_MAIN_2_ID, MENU_SUB_1_0_ID, MENU_MAIN_1_ID},
        {hoofdmenu, speelunigames, "", ""},
        NULL},
    {
        // Sub screens for 1
        MENU_SUB_1_0_ID,
        {MENU_SUB_1_0_ID, MENU_SUB_1_1_ID, MENU_SUB_1_0_0_ID, MENU_MAIN_1_ID}, // edit enter
        {"===UNIGAMES===", "Talking Bas", "", ""},
        NULL},
    {
        // Sub screens for 1
        MENU_SUB_1_1_ID,
        {MENU_SUB_1_0_ID, MENU_SUB_1_2_ID, MENU_SUB_1_1_ID, MENU_MAIN_1_ID},
        {"===UNIGAMES===", "Russian Roulette", "", ""},
        NULL},
    {
        // Sub screens for 1
        MENU_SUB_1_2_ID,
        {MENU_SUB_1_1_ID, MENU_SUB_1_2_ID, MENU_SUB_1_2_ID, MENU_MAIN_1_ID},
        {"===UNIGAMES===", "Coin Flip", "", ""},
        NULL},
    {
        // Main screen
        MENU_MAIN_2_ID,
        {MENU_MAIN_1_ID, MENU_MAIN_2_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
        {hoofdmenu, instellingen1, "", ""},
        NULL},
    {
        // Sub screens for 2
        MENU_SUB_2_0_ID,
        {MENU_SUB_2_0_ID, MENU_SUB_2_1_ID, MENU_SUB_2_0_ID, MENU_MAIN_2_ID},
        {instellingen, hierkanjede, instellingenvande, klokaanpassen}, 
        NULL},
    {
        // Sub screen for 1_0
        MENU_SUB_1_0_0_ID,
        {MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_ID},
        {"===TALKING BAS===", baswacht, vraag, ""},
        NULL},
    {
        // Sub screen for 1_0_0
        MENU_SUB_1_0_1_ID,
        {MENU_SUB_1_0_1_ID, MENU_SUB_1_0_1_ID, MENU_SUB_1_0_2_ID, MENU_SUB_1_0_1_ID},
        {"===TALKING BAS===", basdenktna, "", ""},
        NULL},
    {
        // Sub screen for 1_0_1
        MENU_SUB_1_0_2_ID,
        {MENU_SUB_1_0_2_ID, MENU_SUB_1_0_0_ID, MENU_SUB_1_0_0_ID},
        {"===TALKING BAS===", basgeeftantwoord, reponse, ""},
        NULL},
    {
        // Sub screen for 2_1
        MENU_SUB_2_1_ID,
        {MENU_SUB_2_0_ID, MENU_SUB_2_2_ID, MENU_SUB_2_1_0_ID, MENU_MAIN_2_ID},
        {instellingen, sprekendklokje, "volume instellen", ""},
        NULL},
    {
        // Sub screen for 2_1_0
        MENU_SUB_2_1_0_ID,
        {MENU_SUB_2_1_0_ID, MENU_SUB_2_1_0_ID, MENU_SUB_2_1_0_ID, MENU_SUB_2_1_ID},
        {instellingen, "Volume spr. klok", "", ""},
        increase_volume},
    {
        // Sub screen for 2_2_0
        MENU_SUB_2_2_ID,
        {MENU_SUB_2_1_ID, MENU_SUB_2_2_ID, MENU_SUB_2_0_0_ID, MENU_MAIN_2_ID},
        {instellingen, taalinstellen, "", ""},
        NULL},
    {
        // Sub screen for 2_0_0
        MENU_SUB_2_0_0_ID,
        {MENU_SUB_2_0_0_ID, MENU_SUB_2_0_1_ID, MENU_SUB_2_0_0_ID, MENU_SUB_2_2_ID},
        {instellingen, "Nederlands", "", ""},
        handle_language},
    {
        // Sub screen for 2_0_1
        MENU_SUB_2_0_1_ID,
        {MENU_SUB_2_0_0_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_0_1_ID, MENU_SUB_2_2_ID},
        {instellingen, "English", "", ""},
        handle_language},
    {
        // Sub screen for 2_0_2
        MENU_SUB_2_0_2_ID,
        {MENU_SUB_2_0_1_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_0_2_ID, MENU_SUB_2_2_ID},
        {instellingen, "Francais", "", ""},
        handle_language},
};

// void init_interface() {
//     *instellingen = malloc(sizeof(char) * 20);
//     *instellingen = instellingen_NL;
// }

static unsigned int current_menu_index = MENU_MAIN_0_ID;
static unsigned int current_menu_id;

void talking_bas_random() {
    ESP_LOGE(TAG, "Talking Bas");
}

void print_menu_item(char **text[]);
void clear_menu();
void write_time();

void menu_start() {
    print_menu_item(menu[current_menu_index].text);
}

void handle_menu(int key) {
    switch (key) {
        // up
        case REC_BUTTON_ID:
            current_menu_id = menu[current_menu_index].new_id[0];
            break;
        // back
        case SET_BUTTON_ID:
            current_menu_id = menu[current_menu_index].new_id[3];

            if (current_menu_id == MENU_SUB_1_0_ID) {
                ESP_LOGE(TAG, "Mic stopped");
                mic_stop();
            }
            break;
        // enter
        case PLAY_BUTTON_ID:
            current_menu_id = menu[current_menu_index].new_id[2];
            if (current_menu_id == MENU_SUB_1_0_0_ID) {
                ESP_LOGE(TAG, "Mic initialized");
                mic_init(talking_bas_random);
            }
            break;

        // down
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

void print_menu_item(char **text[]) {
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
    } else if (player.volume < 0) {
        player.volume = 0;
    }

    sprintf(buffer, "Volume: %d", player.volume);
    lcd_clear(3);
    lcd_write(buffer, 0, 3, false);
}

void handle_language(int key) {
    switch (key) {
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
            strcpy(instellingen, instellingen_NL);
            strcpy(hoofdmenu, hoofdmenu_NL);
            strcpy(sprekendeklok, sprekendeklok_NL);
            strcpy(sprekendklokje, sprekendklokje_NL);
            strcpy(timeistold, timeistold_NL);
            strcpy(speelunigames, speelunigames_NL);
            strcpy(instellingen1, instellingen1_NL);
            strcpy(hierkanjede, hierkanjede_NL);
            strcpy(instellingenvande, instellingenvande_NL);
            strcpy(klokaanpassen, klokaanpassen_NL);
            strcpy(baswacht, baswacht_NL);
            strcpy(vraag, vraag_NL);
            strcpy(basdenktna, basdenktna_NL);
            strcpy(basgeeftantwoord, basgeeftantwoord_NL);
            strcpy(reponse, reponse_NL);
            strcpy(taalinstellen, taalinstellen_NL);
            ESP_LOGE("Language", "Language set to Dutch");
            lcd_clear(2);
            lcd_clear(3);
            lcd_write("Taal ingesteld op", 0, 2, false);
            lcd_write("Nederlands", 0, 3, false);
            break;

        case MENU_SUB_2_0_1_ID:
            language = LANGUAGE_ENGLISH;
            strcpy(instellingen, instellingen_EN);
            strcpy(hoofdmenu, hoofdmenu_EN);
            strcpy(sprekendeklok, sprekendeklok_EN);
            strcpy(sprekendklokje, sprekendklokje_EN);
            strcpy(timeistold, timeistold_EN);
            strcpy(speelunigames, speelunigames_EN);
            strcpy(instellingen1, instellingen1_EN);
            strcpy(hierkanjede, hierkanjede_EN);
            strcpy(instellingenvande, instellingenvande_EN);
            strcpy(klokaanpassen, klokaanpassen_EN);
            strcpy(baswacht, baswacht_EN);
            strcpy(vraag, vraag_EN);
            strcpy(basdenktna, basdenktna_EN);
            strcpy(basgeeftantwoord, basgeeftantwoord_EN);
            strcpy(reponse, reponse_EN);
            strcpy(taalinstellen, taalinstellen_EN);
            ESP_LOGE("Language", "Language set to English");
            lcd_clear(2);
            lcd_clear(3);
            lcd_write("Language set to", 0, 2, false);
            lcd_write("English", 0, 3, false);
            break;

        case MENU_SUB_2_0_2_ID:
            language = LANGUAGE_FRENCH;
            strcpy(instellingen, instellingen_FR);
            strcpy(hoofdmenu, hoofdmenu_FR);
            strcpy(sprekendeklok, sprekendeklok_FR);
            strcpy(sprekendklokje, sprekendklokje_FR);
            strcpy(timeistold, timeistold_FR);
            strcpy(speelunigames, speelunigames_FR);
            strcpy(instellingen1, instellingen1_FR);
            strcpy(hierkanjede, hierkanjede_FR);
            strcpy(instellingenvande, instellingenvande_FR);
            strcpy(klokaanpassen, klokaanpassen_FR);
            strcpy(baswacht, baswacht_FR);
            strcpy(vraag, vraag_FR);
            strcpy(basdenktna, basdenktna_FR);
            strcpy(basgeeftantwoord, basgeeftantwoord_FR);
            strcpy(reponse, reponse_FR);
            strcpy(taalinstellen, taalinstellen_FR);
            ESP_LOGE("Language", "Language set to French");
           lcd_clear(2);
            lcd_clear(3);
            lcd_write("Langue reglee sur", 0, 2, false);
            lcd_write("Francais", 0, 3, false);
            break;

        default:
            break;
    }
}
