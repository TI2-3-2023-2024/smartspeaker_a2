#include "../clock/clock_man.h"
#include "../time/time_man.h"

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
        tell_time(get_time());
        break;
        case 8:
        printf("Deze knop heeft geen functie\n");
        break;
    }
} 