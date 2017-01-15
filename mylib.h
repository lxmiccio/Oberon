#include <stdint.h>

typedef enum MonsterType {
    NONE = 0,
    DRAGON,
    OGRE,
    SKELETON,
    WOLF
} MonsterType_t;

typedef enum AreaType {
    HOME = 0,
    DESERT,
    FOREST,
    PLAIN,
    SWAMP,
    VILLAGE
} AreaType_t;

typedef struct Monster {
    MonsterType_t type;
    const char* name;
    int16_t hp;
    uint16_t damages;
} Monster_t;

typedef struct Area {
    AreaType_t type;
    Monster_t monster;
    uint16_t gold;
    struct Area* next;
} Area_t;

typedef struct Oberon {
    int16_t hp;
    uint16_t spells;
    uint16_t potions;
    uint16_t gold;
    bool hasDestroyedArea;
} Oberon_t;

void crea_percorso();
void muovi_oberon();
void termina_gioco();



typedef enum {
    DEFAULT = 0,
    BLUE,
    CYANO,
    GREEN,
    MAGENTA,
    RED,
    WHITE,
    YELLOW
} Color_t;

typedef struct SerializableArea {
    AreaType_t areaType;
    MonsterType_t monsterType;
    uint16_t gold;
} SerializableArea_t;

uint16_t scanfWrap(char* print, uint16_t min, uint16_t max);
void renameFile(void);
void loadFile(void);
void setColor(Color_t color);

