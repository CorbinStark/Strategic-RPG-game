#pragma once
#include "globals.h"
#include <vector>

#define BUILDING_HEAL_AMOUNT 5
#define LARGE_BUILDING_HEAL_AMOUNT 10
#define PRODUCTION_BUILDING_MONEY_GAIN 100
#define MAX_MONEY 9999

enum UnitType {
	//rebels
	UNIT_TYPE_ELEANOR, //special campaign
	UNIT_TYPE_ALEKSANDER_BONES, //special campaign unit
	UNIT_TYPE_RECRUIT,
	UNIT_TYPE_SLAYER, //DOUBLE ATTACK AND ARMOR PIERCING
	UNIT_TYPE_MEDIC,
	UNIT_TYPE_ARTILLERY,
	UNIT_TYPE_TANK,
	UNIT_TYPE_MEGATANK,
	UNIT_TYPE_SCOUT,
	UNIT_TYPE_BOMBER, //flying unit
	UNIT_TYPE_FIGHTER, //flying unit
	//monsters
	UNIT_TYPE_VAMPIRE,
	UNIT_TYPE_GAY_FROG,
	UNIT_TYPE_LIZARD,
	UNIT_TYPE_ALIEN,
	UNIT_TYPE_LASERTANK,
	UNIT_TYPE_UFO, //flying unit
	UNIT_TYPE_MOTHERSHIP, //flying unit
	//globalists
	UNIT_TYPE_HILLARY, //special campaign unit
	UNIT_TYPE_MINDSLAVE, //can be trained from normal towns
	UNIT_TYPE_OFFICER,
	UNIT_TYPE_DEMON,
	UNIT_TYPE_IMP,
	UNIT_TYPE_HELLSPAWN, //flying unit
	//builder
	UNIT_TYPE_BUILDER,

	UNIT_TYPE_COUNT,
	UNIT_TYPE_NONE
};

enum Owner {
	OWNER_GREEN,
	OWNER_RED,
	OWNER_BLUE,
	OWNER_YELLOW,
	OWNER_NEUTRAL,
	OWNER_COUNT
};

//all buildings heal a unit on them
enum BuildingType {
	//main building
	BUILDING_TYPE_COMMAND_CENTER,
	//makes ground biological units
	BUILDING_TYPE_BARRACKS,
	//makes ground mech units
	BUILDING_TYPE_FACTORY,
	//makes air units
	BUILDING_TYPE_AIRPORT,
	//generates money
	BUILDING_TYPE_EXTRACTOR,
	//makes money
	BUILDING_TYPE_PRODUCTION,
	BUILDING_TYPE_COUNT,
	BUILDING_TYPE_NONE
};

enum Attributes {
	ATTR_NONE = 0,
	ATTR_DOUBLE_ATTACK = 1,
	ATTR_HEALER = 2,
	ATTR_FLYING = 4,
	ATTR_ARMOR = 8,
	ATTR_PIERCING = 16,
	ATTR_CAMPAIGN_UNIT = 32,
	ATTR_ARTILLERY = 64,
	ATTR_VAMPIRE = 128,
	ATTR_BUILDER = 256
};

struct HealthBar {
	f32 max;
	f32 current;
	f32 shown;
};

struct Building;

struct Commander {

};

enum FactionType {
	FACTION_REBELS,
	FACTION_MONSTERS,
	FACTION_GLOBALIST,
	FACTION_COUNT
};
struct Unit;

struct UnitTypeData {
	HealthBar health;
	Rect src;

	i16 damage;
	u16 cost;
	u16 speed;
	u16 moveRange;
	u16 attackRange;
	u64 attributes;
	std::string name;
	UnitType type;
};

struct BuildingTypeData {
	Rect src;
	u16 cost;
	std::vector<UnitTypeData> trainable;
	std::string name;
	BuildingType type;
};

struct Building {
	i16 x;
	i16 y;
	BuildingType type;
	Owner owner;
	Rect src;

	Unit* capturing;
	bool active;
};

struct Unit {
	i16 x;
	i16 y;
	std::vector<vec2> path;

	HealthBar health;
	Owner owner;
	Rect src;

	u64 attributes;
	i16 damage;
	u16 moveRange;
	u16 attackRange;
	bool active;
	UnitType type;
};

struct Faction {
	std::vector<UnitTypeData> units;
	std::vector<BuildingTypeData> buildings;
	std::vector<Commander> commanders;
	std::string name;
	FactionType type;
};

enum Personality {
	PERSONALITY_AGGRESSIVE,
	PERSONALITY_DEFENSIVE,
	PERSONALITY_BALANCED,
	PERSONALITY_GREEDY,
	PERSONALITY_NOBUILD
};

struct Player {
	vec4 color;
	Faction faction;
	Personality personality;
	std::vector<Unit> units;
	std::vector<Building> buildings;
	i32 money = 0;
	//enemies are everyone that isn't an ally
	bool allied[OWNER_COUNT] = { 0 };
	bool AI = false;

	u16 lastSelectedUnit = 0;
	vec2 nextBuilding;
};

void preload_all_unit_and_building_types();
Unit create_unit(UnitType type, Owner owner, u32 x, u32 y);
Building create_building(BuildingType type, Owner owner, u32 x, u32 y);
UnitTypeData get_type_data(UnitType type);
BuildingTypeData get_type_data(BuildingType type);
Faction get_faction_from_type(FactionType type);