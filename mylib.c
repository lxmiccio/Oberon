#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mylib.h"

static Area_t* path = NULL;
static Area_t* last = NULL;

static Oberon_t oberon;
static Area_t* current = NULL;

static bool pathCreated = false;

static void initializeMonster(Monster_t* monster);

static void ins_terra(void);
static void canc_terra(void);
static void stampa_percorso(void);
static void chiudi_percorso(void);

static void avanza(void);
static void prendi_tesoro(void);
static void usa_pozione(void);
static void combatti(void);
static void distruggi_terra(void);

static const char* areaTypes[] = { "Home", "Desert", "Forest", "Plain", "Swamp", "Village" };
static const uint8_t areaTypesCount = 6U;

static const char* monsterTypes[] = { "None", "Dragon", "Ogre", "Skeleton", "Wolf" };
static const uint8_t monsterTypesCount = 4U; /* None is not a monster */

void crea_percorso(void)
{
	pathCreated = false;
	
	do {
		setColor(CYANO);

		printf("**************************************************\n");
		printf("[1] Inserisci nuova terra\n");
		printf("[2] Rimuovi ultima terra\n");
		printf("[3] Stampa percorso\n");
		printf("[4] Torna al menu\n\n");

		setColor(DEFAULT);
		
		printf("Scelta: ");
		
		uint16_t choice;
		scanf("%hu", &choice);
		
		printf("\n");
		
		switch(choice)
		{
			case 1U:
				ins_terra();
				break;

			case 2U:
				canc_terra();
				break;

			case 3U:
				stampa_percorso();
				break;

			case 4U:
				savePath();
				chiudi_percorso();
				break;

			default:
				while(getchar() != '\n');
				break;
		}

	} while(!pathCreated);
}

static void initializeMonster(Monster_t* monster)
{
	switch(monster->type)
	{
		case NONE:
			monster->name = "None";
			monster->hp = 0U;
			monster->damages = 0U;
			break;
		
		case DRAGON:
			monster->name = "Dragon";
			monster->hp = 5U;
			monster->damages = 5U;
			break;
		
		case OGRE:
			monster->name = "Ogre";
			monster->hp = 3U;
			monster->damages = 3U;
			break;
		
		case SKELETON:
			monster->name = "Skeleton";
			monster->hp = 2U;
			monster->damages = 2U;
			break;
		
		case WOLF:
			monster->name = "Wolf";
			monster->hp = 1U;
			monster->damages = 1U;
			break;

		default:
			break;
	}
}

static void ins_terra(void)
{
	/* Allocate a new area */
	Area_t* area = (Area_t*) malloc(sizeof(Area_t));
	
	/* Initialize next to NULL */
	area->next = NULL;
	
	/* Ask type */
	while(true)
	{
		int16_t i;
		for(i = 0; i < areaTypesCount; i++)
		{
			i < areaTypesCount - 1 ? printf("[%hu] %s\n", i, areaTypes[i]) : printf("[%hu] %s\n\n", i, areaTypes[i]);
		}

        	printf("Area type: ");
		scanf("%d", &(area)->type);
		printf("\n");
		
		if(area->type >= 0 && area->type < areaTypesCount)
		{
			break;
		}
	}
	
	/* Ask monster */
	while(true)
	{
		int16_t i;
		for(i = 0; i <= monsterTypesCount; i++)
		{
			i <= monsterTypesCount - 1 ? printf("[%hu] %s\n", i, monsterTypes[i]) : printf("[%hu] %s\n\n", i, monsterTypes[i]);
		}
		
		printf("Monster type: ");
		scanf("%d", &(area)->monster.type);
		printf("\n");

		area->monster.type--;

		if(((path == NULL || area->type == HOME || area->type == VILLAGE) && area->monster.type != NONE)
			|| (area->type == SWAMP && area->monster.type == SKELETON)
			|| (area->type == DESERT && area->monster.type == OGRE)
			|| (area->monster.type == WOLF && area->type != FOREST && area->type != PLAIN)
			|| (area->monster.type > monsterTypesCount))
		{
			/* Invalid choice */
		}
		else
		{
			initializeMonster(&(area)->monster);

			break;
		}
	}
	
	/* Ask gold */
	while(true)
	{
		printf("Gold: ");
		scanf("%hu", &(area)->gold);
		printf("\n");
		
		if(area->type != VILLAGE || (area->type == VILLAGE && area->gold <= 10))

		{
			break;
		}
	}
	
	/* Update path if the area created is the first */
	if(path == NULL)
	{
		path = area;
	}
	
	/* Update next of the last area to the new one */
	if(last != NULL)
	{
		last->next = area;
	}
	
	/* Update last to the new area */
	last = area;
}

static void canc_terra(void)
{
	if(path != NULL && last != NULL)
	{
		/* Set path and last to NULL if the first area has been deleted */
		if(path == last)
		{
			free(path);

			last = NULL;
			path = NULL;
		}
		else
		{
			Area_t* temp = path;
		
			/* Loop through the path */
			while(temp->next != last)
			{
				temp = temp->next;
			}
		
			free(last);
		
			/* Update the last area */
			last = temp;
			last->next = NULL;
		}
	}
}

static void stampa_percorso(void)
{
	if(path != NULL)
	{
		Area_t* temp = path;
	
		int16_t i = 0;
	
		do {
			printf("Area {%hu, %s, %s, %hu}\n", i++, areaTypes[temp->type], temp->monster.name, temp->gold);

		} while((temp = temp->next) != NULL);

		printf("\n");
	}
}

static void chiudi_percorso(void)
{
	pathCreated = true;
}



void muovi_oberon(void)
{
	/* Return if path hasn't been created */
	if(!pathCreated)
	{
		return;
	}
	
	/* Initialize oberon */
	oberon.hp = 5U;
	oberon.spells = 2U;
	oberon.potions = 1U;
	oberon.gold = 10U;
	oberon.hasDestroyedArea = false;
	
	/* Initialize current */
	current = path;
	
	do
	{
		setColor(CYANO);

		printf("**************************************************\n");
		printf("[1] Move forward\n");
		printf("[2] Gather gold (%hu left)\n", current->gold);
		printf("[3] Use healing potion (%hu left)\n", oberon.potions);
		if(current->monster.type == NONE)
		{
			printf("[4] No monster to fight\n", current->monster.name);
		}
		else
		{
			printf("[4] Fight %s (%hu hp left)\n", current->monster.name, current->monster.hp);
		}
		printf("[5] Destroy next area\n\n");

		setColor(DEFAULT);
		
		uint16_t choice;
		
		printf("Choice: ");
		scanf("%hu", &choice);
		printf("\n");
		
		switch(choice)
		{
			case 1U:
			avanza();
			break;
			
			case 2U:
			prendi_tesoro();
			break;
			
			case 3U:
			usa_pozione();
			break;
			
			case 4U:
			combatti();
			break;
			
			case 5U:
			distruggi_terra();
			break;
			
			default:
				while(getchar() != '\n');
			break;
		}
		
	}
	while(oberon.hp > 0 && current != NULL);

	termina_gioco();
}

static void avanza(void)
{
	if(current == NULL)
	{
		return;
	}

	/* Exit if a monster is present */
	if(current->monster.type == DRAGON && current->monster.hp > 0)
	{
		printf("You have to defeat the %s to move forward\n\n", current->monster.name);

		return;
	}

	/* Move forward */
	current = current->next;
}

static void prendi_tesoro(void)
{
	if(current != NULL)
	{
	    /* Exit if a monster is present */
	    if(current->monster.hp <= 0)
	    {
	        /* Add gold gathered */
	        oberon.gold += current->gold;

	        /* Set gold to 500 if it is greater than maximum */
	        if(oberon.gold > 500)
	        {
	            oberon.gold = 500;
	        }

		current->gold = 0;
	    }
	    else
	    {
            	printf("You have to defeat the %s to gether the gold\n\n", current->monster.name);
	    }
	}
}

static void usa_pozione(void)
{
	/* Use potion only if available */
	if(oberon.potions > 0)
	{
		oberon.potions--;
		oberon.hp += 5U;
		
		printf("Healing potion used (%d left)\n", oberon.potions);
		printf("Now Oberon has %d hp\n\n", oberon.hp);
	}
	else
	{
		printf("No healing potions available\n\n");
	}
}

static void combatti(void)
{
	if(current == NULL)
	{
		return;
	}
	
	/* Return if there are no monsters */
	if(current->monster.type < 0 || current->monster.type >= monsterTypesCount)
	{
		printf("No monsters in this area\n\n");

		return;
	}

	while(oberon.hp > 0 && current->monster.hp > 0)
	{
		setColor(CYANO);

		printf("**************************************************\n");
		printf("[1] Attack\n");
		printf("[2] Cast Spell (%hu left)\n\n", oberon.spells);

		setColor(DEFAULT);
		
		uint16_t choice;
		
		printf("Choice: ");
		scanf("%hu", &choice);
		printf("\n");
		
		switch(choice)
		{
			case 1:
			/* Oberon turn */
			if(rand() % 100 >= 60)
			{
				printf("Oberon attacks\n");

				current->monster.hp -= 3;
				
				/* Monster is death */
				if(current->monster.hp <= 0)
				{
					current->monster.hp = 0;
					current->monster.type = NONE;

					printf("%s has been defeated\n\n", current->monster.name);

					break;
				}
			}
			else
			{
				printf("Oberon fails\n");
			}
			
			printf("%s has %hu hp left\n\n", current->monster.name, current->monster.hp);
			
			/* Monster turn */
			if(rand() % 100 >= 50)
			{
				printf("%s attacks\n", current->monster.name);

				oberon.hp -= current->monster.damages;
				
				/* Oberon is death */
				if(oberon.hp <= 0)
				{
					oberon.hp = 0;
					printf("Oberon has been defeated\n\n");

					break;
				}
			}
			else
			{
				printf("Monster fails\n");
			}
			
			printf("Oberon has %hu hp left\n\n", oberon.hp);
			
			break;
			
			case 2:
			/* Cast spell only if available */
			if(oberon.spells > 0)
			{
				oberon.spells--;
				
				printf("Spell used (%hu left)\n", oberon.spells);
				printf("%s has been defeated\n\n", current->monster.name);
				
				current->monster.hp = 0;
				current->monster.type = NONE;
			}
			else
			{
				printf("No spells available\n\n");
			}
			
			break;
			
			default:
				while(getchar() != '\n');
			break;
		}
		
	}
}

static void distruggi_terra(void)
{
	/* Return if oberon has already destroyed an area */
	if(oberon.hasDestroyedArea)
	{
		printf("Oberon has already destroyed an area\n\n");
		
		return;
	}

	/* Destroy the next area only if it exists */
	if(current->next != NULL)
	{
		printf("Oberon has destroyed the next area\n\n");

		oberon.hasDestroyedArea = true;

		Area_t* temp = current->next;

		/* Link the current area to the next one */
		current->next = temp->next;

		/* Deallocate the destroyed area */
		free(temp);
	}
}


void termina_gioco(void)
{
	if(path != NULL)
	{
	    /* Loop through the path and deallocate all the areas */
	    while(path->next != NULL)
	    {
	        Area_t* temp = path;

	        path = path->next;

	        free(temp);
	    }

	    path = NULL;
	}

	exit(0);
}

void savePath()
{
	if(path != NULL)
	{
		FILE* file = fopen("temp.sav", "wb");

		if(file != NULL)
		{
			Area_t* temp = path;

			do {
				SerializableArea_t serializable;
				serializable.areaType = temp->type;
				serializable.monsterType = temp->monster.type;
				serializable.gold = temp->gold;

				fwrite(&serializable, sizeof(SerializableArea_t), 1, file);

			} while((temp = temp->next) != NULL);

			fclose(file);
		}
	}
}

void renameFile()
{
	char filename[256];

	printf("Filename: ");
	scanf("%s", filename);
	printf("\n");

	rename("temp.sav", filename);
}

void loadFile()
{
	char filename[256];

	printf("Filename: ");
	scanf("%s", filename);
	printf("\n");

	FILE* file = fopen(filename, "rb");

	if(file != NULL)
	{
		while(true)
		{
			SerializableArea_t* serializable = (SerializableArea_t*) malloc(sizeof(SerializableArea_t));

	    		Area_t* area = (Area_t*) malloc(sizeof(Area_t));
			area->next = NULL;
			
			int16_t res = fread(serializable, sizeof(SerializableArea_t), 1, file);

			if(res != 1)
			{
				break;
			}

			area->type = serializable->areaType;
			area->monster.type = serializable->monsterType;
			initializeMonster(&(area)->monster);

			area->gold = serializable->gold;

			/* Update path if the area read is the first */
			if(path == NULL)
			{
				path = area;

				/* Set pathCreated to true if at least an area has been read */
				chiudi_percorso();
			}

			/* Update next of the last area to the new one */
			if(last != NULL)
			{
				last->next = area;
			}

			/* Update last to the new area */
			last = area;
		}

		fclose(file);
	}

}

void setColor(Color_t color)
{
	char* code;

	switch(color)
	{
		case DEFAULT:
			code = "\x1B[0m";
			break;
		case BLUE:
			code = "\x1B[34m";
			break;
		case CYANO:
			code = "\x1B[36m";
			break;
		case GREEN:
			code = "\x1B[32m";
			break;
		case MAGENTA:
			code = "\x1B[35m";
			break;
		case RED:
			code = "\x1B[31m";
			break;
		case WHITE:
			code = "\x1B[37m";
			break;
		case YELLOW:
			code = "\x1B[33m";
			break;
		default:
			code = "\x1B[0m";
			break;
	}

	printf("%s", code);
}

