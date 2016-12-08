#include <stdint.h>

typedef enum MonsterType {
	NONE = -1,
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
	uint16_t hp;
	uint16_t damages;
} Monster_t;

typedef struct Area {
	AreaType_t type;
	Monster_t monster;
	uint16_t gold;
	struct Area* next;
} Area_t;

typedef struct Oberon {
	uint16_t hp;
	uint16_t spells;
	uint16_t potions;
	uint16_t gold;
	bool hasDestroyedArea;
} Oberon_t;

