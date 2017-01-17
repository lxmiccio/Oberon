#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mylib.h"

static void ins_terra(void);
static void canc_terra(void);
static void stampa_percorso(void);
static void chiudi_percorso(void);

static void avanza(void);
static void prendi_tesoro(void);
static void usa_pozione(void);
static void combatti(void);
static void distruggi_terra(void);

static void initializeMonster(Monster_t* monster);
static void savePath(void);

static Area_t* path = NULL;
static Area_t* last = NULL;
static Area_t* current = NULL;

static bool pathCreated = false;

static Oberon_t oberon;

static const char* areaTypes[] = { "Home", "Desert", "Forest", "Plain", "Swamp", "Village" };
static const char* monsterTypes[] = { "None", "Dragon", "Ogre", "Skeleton", "Wolf" };



void crea_percorso(void)
{
    pathCreated = false;

    while(!pathCreated)
    {
        puts("**************************************************");
        puts("[1] Add an area");
        puts("[2] Remove last area");
        puts("[3] Print path");
        puts("[4] Back to menu\n");

        switch(scanfWrap("Choice: ", 1, 4))
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
            chiudi_percorso();
            break;

        default:
            break;
        }
    }
}

static void ins_terra(void)
{
    /* Allocate a new area */
    Area_t* area = (Area_t*) malloc(sizeof(Area_t));
    area->next = NULL;

    /* Ask type */
    for(int16_t i = HOME; i <= VILLAGE; i++)
    {
        i < VILLAGE ? printf("[%hu] %s\n", i + 1, areaTypes[i]) : printf("[%hu] %s\n\n", i + 1, areaTypes[i]);
    }

    area->type = scanfWrap("Choice: ", HOME + 1, VILLAGE + 1) - 1;


    /* Ask monster */
    while(true)
    {
        for(int16_t i = NONE; i <= WOLF; i++)
        {
            i < WOLF ? printf("[%hu] %s\n", i + 1, monsterTypes[i]) : printf("[%hu] %s\n\n", i + 1, monsterTypes[i]);
        }

        area->monster.type = scanfWrap("Choice: ", NONE + 1, WOLF + 1) - 1;

        if(((path == NULL || area->type == HOME || area->type == VILLAGE) && area->monster.type != NONE)
                || (area->type == SWAMP && area->monster.type == SKELETON)
                || (area->type == DESERT && area->monster.type == OGRE)
                || (area->monster.type == WOLF && area->type != FOREST && area->type != PLAIN)
                || (area->monster.type > WOLF))
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
    area->gold = scanfWrap("Gold: ", 0, area->type == VILLAGE ? 10 : UINT16_MAX);

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

        for(int16_t i = 0; temp != NULL; i++, temp = temp->next)
        {
            printf("Area {%hu, %s, %s, %hu}\n", i, areaTypes[temp->type], monsterTypes[temp->monster.type], temp->gold);
        }
        printf("\n");
    }
}

static void chiudi_percorso(void)
{
    pathCreated = true;
    savePath();
}



void muovi_oberon(void)
{
    if(pathCreated)
    {
        /* Initialize oberon */
        oberon.hp = 5;
        oberon.spells = 2U;
        oberon.potions = 1U;
        oberon.gold = 10U;
        oberon.hasDestroyedArea = false;

        /* Initialize current */
        current = path;

        while(oberon.hp > 0 && current != NULL)
        {
            puts("**************************************************");
            puts("[1] Move forward");
            printf("[2] Gather gold (%hu left)\n", current->gold);
            printf("[3] Use healing potion (%hu left)\n", oberon.potions);
            current->monster.type != NONE ? printf("[4] Fight %s (%hu hp left)\n", current->monster.name, oberon.hp) : puts("[4] No monster to fight");
            puts("[5] Destroy next area\n");

            switch(scanfWrap("Choice: ", 1, 5))
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
                break;
            }
        }

        termina_gioco();
    }
}

static void avanza(void)
{
    if(current != NULL)
    {
        if(current->monster.type == DRAGON)
        {
            printf("You have to defeat the %s to move forward\n\n", current->monster.name);
        }
        else
        {
            /* Move forward */
            current = current->next;
        }
    }
}

static void prendi_tesoro(void)
{
    if(current != NULL)
    {
        if(current->monster.type == NONE)
        {
            /* Add gold gathered */
            oberon.gold += current->gold;

            /* Set gold to 500 if greater than maximum */
            if(oberon.gold > 500)
            {
                oberon.gold = 500;
            }

            /* Remove gold from current area */
            current->gold = 0;
        }
        else
        {
            printf("You have to defeat the %s to gather the gold\n\n", current->monster.name);
        }
    }
}

static void usa_pozione(void)
{
    /* Use potion only if available */
    if(oberon.potions > 0)
    {
        oberon.hp = 5U;
        oberon.potions--;

        printf("Healing potion used (%d left)\n", oberon.potions);
        printf("Now Oberon has %d hp\n\n", oberon.hp);
    }
    else
    {
        puts("No healing potions available\n");
    }
}

static void combatti(void)
{
    if(current != NULL)
    {
        if(current->monster.type != NONE)
        {
            while(oberon.hp > 0 && current->monster.hp > 0)
            {
                puts("**************************************************");
                puts("[1] Attack");
                printf("[2] Cast Spell (%hu left)\n\n", oberon.spells);

                switch(scanfWrap("Choice: ", 1, 2))
                {
                case 1:
                    /* Oberon turn */
                    if(rand() % 100 >= 60)
                    {
                        puts("Oberon attacks");

                        current->monster.hp -= 3;

                        /* Check if monster is dead */
                        if(current->monster.hp <= 0)
                        {
                            printf("The %s has been defeated\n\n", current->monster.name);

                            current->monster.hp = 0;
                            current->monster.type = NONE;
                            break;
                        }
                    }
                    else
                    {
                        puts("Oberon fails");
                    }

                    printf("The %s has %hu hp left\n\n", current->monster.name, current->monster.hp);

                    /* Monster turn */
                    if(rand() % 100 >= 50)
                    {
                        printf("The %s attacks\n", current->monster.name);

                        oberon.hp -= current->monster.damages;

                        /* Check if Oberon is dead */
                        if(oberon.hp <= 0)
                        {
                            puts("Oberon has been defeated\n");

                            oberon.hp = 0;
                            break;
                        }
                    }
                    else
                    {
                        puts("Monster fails");
                    }

                    printf("Oberon has %hu hp left\n\n", oberon.hp);

                    break;

                case 2:
                    /* Cast spell only if available */
                    if(oberon.spells > 0)
                    {
                        printf("Spell used (%hu left)\n", oberon.spells);
                        printf("%s has been defeated\n\n", current->monster.name);

                        oberon.spells--;

                        current->monster.hp = 0;
                        current->monster.type = NONE;
                    }
                    else
                    {
                        puts("No spells available\n");
                    }

                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            puts("No monster in this area\n");
        }
    }
}

static void distruggi_terra(void)
{
    if(current != NULL && current->next != NULL)
    {
        if(!oberon.hasDestroyedArea)
        {
            puts("Oberon has destroyed the next area\n");

            oberon.hasDestroyedArea = true;

            /* Link the current area to the next one */
            Area_t* temp = current->next;
            current->next = temp->next;

            /* Deallocate the destroyed area */
            free(temp);
        }
        else
        {
            puts("Oberon has already destroyed an area\n");
        }
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
            path = temp->next;
            free(temp);
        }

        path = NULL;

        /* Remove temporary file */
        remove("temp.sav");
    }
}



static void initializeMonster(Monster_t* monster)
{
    if(monster != NULL)
    {
        switch(monster->type)
        {
        case NONE:
            monster->name = "none";
            monster->hp = 0;
            monster->damages = 0U;
            break;

        case DRAGON:
            monster->name = "dragon";
            monster->hp = 5;
            monster->damages = 5U;
            break;

        case OGRE:
            monster->name = "ogre";
            monster->hp = 3;
            monster->damages = 3U;
            break;

        case SKELETON:
            monster->name = "skeleton";
            monster->hp = 2;
            monster->damages = 2U;
            break;

        case WOLF:
            monster->name = "wolf";
            monster->hp = 1;
            monster->damages = 1U;
            break;

        default:
            break;
        }
    }
}

static void savePath(void)
{
    if(path != NULL)
    {
        FILE* file = fopen("temp.sav", "wb");

        if(file != NULL)
        {
            for(Area_t* temp = path; temp != NULL; temp = temp->next)
            {
                SerializableArea_t serializable;
                serializable.areaType = temp->type;
                serializable.monsterType = temp->monster.type;
                serializable.gold = temp->gold;

                fwrite(&serializable, sizeof(SerializableArea_t), 1, file);
            }

            fclose(file);
        }
    }
}

uint16_t scanfWrap(char* print, uint16_t min, uint16_t max)
{
    char string[8];
    uint64_t readNumber; /* Prevent overflow */
    uint16_t number;

    while(true)
    {
        printf("%s", print);
        fgets(string, sizeof(string), stdin);

        bool clear = true;
        bool valid = true;

        for(uint16_t i = 0; i < sizeof(string); i++)
        {
            if(string[i] == '\n')
            {
                clear = false;
                break;
            }

            if(string[i] < '0' || string[i] > '9')
            {
                valid = false;
            }
        }

        if(clear)
        {
            while(getchar() != '\n');
        }

        /* String is valid */
        if(valid)
        {
            readNumber = strtoul(string, NULL, 0);

            if(readNumber >= min && readNumber <= max)
            {
                number = (uint16_t) readNumber;
                break;
            }
        }
    }
    printf("\n");

    return number;
}

void renameFile()
{
    char filename[256];

    printf("Filename: ");
    scanf("%s", filename);
    printf("\n");

    rename("temp.sav", filename);
}

void loadFile(void)
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
            SerializableArea_t serializable;

            if(fread(&serializable, sizeof(SerializableArea_t), 1, file) != 1)
            {
                break;
            }

            Area_t* area = (Area_t*) malloc(sizeof(Area_t));
            area->next = NULL;
            area->type = serializable.areaType;
            area->gold = serializable.gold;

            area->monster.type = serializable.monsterType;
            initializeMonster(&(area)->monster);

            /* Update path if the area read is the first */
            if(path == NULL)
            {
                path = area;

                /* Set pathCreated to true if at least an area has been read */
                pathCreated = true;
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

    if(pathCreated)
    {
        puts("Path loaded\n");
    }
    else
    {
        puts("Cannot load path\n");
    }
}
