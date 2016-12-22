#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mylib.h"

int main(void)
{
    system("clear");

	time_t t;
	srand((unsigned) time(&t));

	bool exit = false;

	do
	{
		printf("**************************************************\n");
		printf("[1] Crea percorso\n");
		printf("[2] Muovi Oberon\n");
		printf("[3] Termina gioco\n\n");

		printf("Scelta: ");

		uint16_t choice;
		scanf("%hu", &choice);

		printf("\n");

		switch(choice)
		{
			case 1:
				crea_percorso();
				break;

			case 2:
				muovi_Oberon();
				break;

			case 3:
				termina_gioco();
				exit = true;
				break;

			default:
				printf("Scelta non esistente\n\n");
				break;
		}		
	}
	while(!exit);

	printf("Partita terminata\n\n");

	return 0;
}
