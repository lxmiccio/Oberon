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

static void ins_terra();
static void canc_terra();
static void stampa_percorso();
static void chiudi_percorso();

static void avanza();
static void prendi_tesoro();
static void usa_pozione();
static void combatti();
static void distruggi_terra();

static const char* areaTypes[] = { "Home", "Desert", "Forest", "Plain", "Swamp", "Village" };
static const uint8_t areaTypesCount = 6;

static const char* monsterTypes[] = { "None", "Dragon", "Ogre", "Skeleton", "Wolf" };
static const uint8_t monsterTypesCount = 4; // None is not a monster

void crea_percorso()
{
	pathCreated = false;
	
	do {
		printf("**************************************************\n");
		printf("[1] Add area\n");
		printf("[2] Remove last area\n");
		printf("[3] Print path\n");
		printf("[4] Back to menu\n");
		printf("\n");
		
		printf("Scelta: ");
		
		uint16_t choice;
		scanf("%hu", &choice);
		
		printf("\n");
		
		switch(choice)
		{
			case 1:
			ins_terra();
			break;
			
			case 2:
			canc_terra();
			break;
			
			case 3:
			stampa_percorso();
			break;
			
			case 4:
			chiudi_percorso();
			break;
			
			default:
			printf("Scelta non esistente\n");
			printf("\n");
			break;
		}
		
	} while(!pathCreated);
}

void initializeMonster(Monster_t* monster)
{
	switch(monster->type)
	{
		case NONE:
		monster->name = "None";
		monster->hp = 0;
		monster->damages = 0;
		break;
		
		case DRAGON:
		monster->name = "Dragon";
		monster->hp = 5;
		monster->damages = 5;
		break;
		
		case OGRE:
		monster->name = "Ogre";
		monster->hp = 3;
		monster->damages = 3;
		break;
		
		case SKELETON:
		monster->name = "Skeleton";
		monster->hp = 2;
		monster->damages = 2;
		break;
		
		case WOLF:
		monster->name = "Wolf";
		monster->hp = 1;
		monster->damages = 1;
		break;
	}
}

void ins_terra()
{
	/* Allocate a new area */
	Area_t* area = malloc(sizeof(Area_t));
	
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
		
		uint16_t choice;
		
		printf("Area type: ");
		scanf("%hu", &choice);
		printf("\n");
		
		if(choice >= 0 && choice < areaTypesCount)
		{
			area->type = choice;
			break;
		}
	}
	
	/* Ask monster */
	while(true)
	{
		int16_t i;
		for(i = -1; i < monsterTypesCount; i++)
		{
			i < monsterTypesCount - 1 ? printf("[%hu] %s\n", i + 1, monsterTypes[i + 1]) : printf("[%hu] %s\n\n", i + 1, monsterTypes[i + 1]);
		}
		
		int16_t choice;
		
		printf("Monster type: ");
		scanf("%hu", &choice);
		printf("\n");
		
		// Decrease choise because MonsterType_t starts from -1, not 0
		choice--;

		bool exit = true;

		if(((path == NULL || area->type == HOME || area->type == VILLAGE) && choice != NONE)
			|| (area->type == SWAMP && choice == SKELETON)
			|| (area->type == DESERT && choice == OGRE)
			|| (choice == WOLF && area->type != FOREST && area->type != PLAIN))
		{
			exit = false;
		}

		if(exit)
		{
			area->monster.type = choice;
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
		
		if(area->gold <= 10)
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

void canc_terra()
{
	if(path == NULL || last == NULL)
	{
		return;
	}
	
	/* Set path and last to NULL if the first area has been deleted */
	if(path == last)
	{
		free(path);
		
		path = NULL;
		last = NULL;
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

void stampa_percorso()
{
	if(path == NULL)
	{
		return;
	}
	
	Area_t* temp = path;
	
	int16_t i = 0;
	
	do
	{
		printf("Area {%hu, %s, %s, %hu}\n", i++, areaTypes[temp->type], temp->monster.name, temp->gold);
	}
	while((temp = temp->next) != NULL);
	
	printf("\n");
}

void chiudi_percorso()
{
	pathCreated = true;
}



void muovi_Oberon()
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
		printf("**************************************************\n");
		printf("[1] Move forward\n");
		printf("[2] Gather gold (%hu left)\n", oberon.gold);
		printf("[3] Use healing potion (%hu left)\n", oberon.potions);
		printf("[4] Fight %s (%hu hp left)\n", current->monster.name, oberon.hp);
		printf("[5] Destroy next area\n\n");
		
		uint16_t choice;
		
		printf("Choice: ");
		scanf("%hu", &choice);
		printf("\n");
		
		switch(choice)
		{
			case 1:
			avanza();
			break;
			
			case 2:
			prendi_tesoro();
			break;
			
			case 3:
			usa_pozione();
			break;
			
			case 4:
			combatti();
			break;
			
			case 5:
			distruggi_terra();
			break;
			
			default:
			printf("Invalid choice\n\n");
			break;
		}
		
	}
	while(oberon.hp > 0 && current != NULL);
}

void avanza()
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

void prendi_tesoro()
{
	if(current == NULL)
	{
		return;
	}
	
	/* Exit if a monster is present */
	if(current->monster.hp > 0)
	{
		printf("You have to defeat the %s to gether the gold\n\n", current->monster.name);
		
		return;
	}
	
	/* Add gold gathered */
	oberon.gold += current->gold;
	
	/* Set gold to 500 if it is greater than maximum */
	if(oberon.gold > 500)
	{
		oberon.gold = 500;
	}
}

void usa_pozione()
{
	/* Use potion only if available */
	if(oberon.potions > 0)
	{
		oberon.potions--;
		oberon.hp += 5;
		
		printf("Healing potion used (%d left)\n", oberon.potions);
		printf("Now Oberon has %d hp\n\n", oberon.hp);
	}
	else
	{
		printf("No healing potions available\n\n");
	}
}

void combatti()
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
		printf("**************************************************\n");
		printf("[1] Attack\n");
		printf("[2] Cast Spell (%hu left)\n\n", oberon.spells);
		
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
					printf("Oberon has been defeated\n");
					printf("\n");
					
					break;
				}
			}
			else
			{
				printf("Monster fails\n");
			}
			
			printf("Oberon has %hu hp left\n", oberon.hp);
			printf("\n");
			
			break;
			
			case 2:
			/* Cast spell only if available */
			if(oberon.spells > 0)
			{
				oberon.spells--;
				
				printf("Spell used (%hu left)\n", oberon.spells);
				printf("%s has been defeated\n\n", current->monster.name);
				
				current->monster.hp = 0;
			}
			else
			{
				printf("No spells available\n\n");
			}
			
			break;
			
			default:
			printf("Invalid choice\n\n");
			break;
		}
		
	}
}

void distruggi_terra()
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


void termina_gioco()
{
	if(path == NULL)
	{
		return;
	}
	
	/* Loop through the path and deallocate all the areas */
	while(path->next != NULL)
	{
		Area_t* temp = path;
		
		path = path->next;
		
		free(temp);
	}
	
	path = NULL;
}

