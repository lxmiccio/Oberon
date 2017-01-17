#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mylib.h"

int main(void)
{
    time_t t;
    srand((unsigned) time(&t));

    bool exit = false;

    while(!exit)
    {
        puts("**************************************************");
        puts("[1] Create path");
        puts("[2] Move oberon");
        puts("[3] End game");
        puts("[4] Save path");
        puts("[5] Load path\n");

        switch(scanfWrap("Choice: ", 1, 5))
        {
        case 1:
            crea_percorso();
            break;

        case 2:
            muovi_oberon();
            break;

        case 3:
            termina_gioco();
            exit = true;
            break;

        case 4:
            renameFile();
            break;

        case 5:
            loadFile();
            break;

        default:
            break;
        }
    }

    puts("**************************************************");
    puts("Game ended\n");

    return 0;
}
