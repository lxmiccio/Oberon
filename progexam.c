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

	do
	{
		setColor(CYANO);

		printf("**************************************************\n");
		printf("[1] Crea percorso\n");
		printf("[2] Muovi Oberon\n");
		printf("[3] Termina gioco\n");
		printf("[4] Salva percorso\n");
		printf("[5] Carica percorso\n\n");

		setColor(DEFAULT);

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
				/* Clear stdin */
				while(getchar() != '\n');

				break;
		}

	} while(!exit);

	printf("Partita terminata\n\n");

	return 0;
}
