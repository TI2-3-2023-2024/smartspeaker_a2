#include "../clock/clock_man.h"
#include "../time/time_man.h"
#include "../audio/audio_man.h"

/// @brief this is a switchcase in which you can define what happens when a button is pressed.
/// @param a 
/// @param case ... is where you can place a pointer to the function you want the button to call,look at button_man.c to see which button calls what case.

extern audio_component_t player;

void set_player(audio_component_t player) {
    player = player;
}

void pressed(int a) { 
    switch(a){
        case 1:
        printf("tab omhoog\n");
        break;
        case 2:
        printf("terug\n");
        break;
        case 3:
        printf("ENTER\n");
        break;
        case 4:
        printf("tab omlaag\n");
        break;
        case 5:
        printf("volume omlaag\n");
        break;
        case 6:
        printf("volume omhoog\n");
        break;
        case 7:
        printf("Klok afspelen\n");
        tell_time(get_time(), player);
        break;
        case 8:
        printf("Deze knop heeft geen functie\n");
        break;
    }
} 