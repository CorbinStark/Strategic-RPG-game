#include "game_objects.h"
#include <string>
#include <SOIL.h>

INTERNAL UnitTypeData UNIT_TYPES[UNIT_TYPE_COUNT];
INTERNAL BuildingTypeData BUILDING_TYPES[BUILDING_TYPE_COUNT];

void preload_all_unit_and_building_types() {
	for (u16 i = 0; i < UNIT_TYPE_COUNT; ++i) {
		UnitTypeData unit;
		unit.type = (UnitType)i;

		if (i == UNIT_TYPE_BUILDER) {
			unit.damage = 1;
			unit.health.current = 8;
			unit.health.max = 8;
			unit.cost = 400;
			unit.health.shown = unit.health.max;
			unit.moveRange = 5;
			unit.attackRange = 1;
			unit.name = "Builder";
			unit.attributes = ATTR_BUILDER;
			unit.src = rect(0, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_ELEANOR) {
			unit.damage = 8;
			unit.health.current = 18;
			unit.health.max = 18;
			unit.cost = 9999;
			unit.health.shown = unit.health.max;
			unit.moveRange = 5;
			unit.attackRange = 1;
			unit.name = "Lizette";
			unit.attributes = ATTR_CAMPAIGN_UNIT;
			unit.src = rect(0, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_VAMPIRE) {
			unit.damage = 4;
			unit.health.current = 10;
			unit.health.max = 10;
			unit.health.shown = unit.health.max;
			unit.cost = 500;
			unit.moveRange = 3;
			unit.attackRange = 1;
			unit.name = "Vampire";
			unit.attributes = ATTR_VAMPIRE;
			unit.src = rect(32, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_LIZARD) {
			unit.damage = 4;
			unit.health.current = 10;
			unit.health.max = 10;
			unit.health.shown = unit.health.max;
			unit.cost = 600;
			unit.moveRange = 3;
			unit.attackRange = 1;
			unit.name = "Lizardman";
			unit.attributes = ATTR_NONE;
			unit.src = rect(32, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_ALEKSANDER_BONES) {
			unit.damage = 4;
			unit.health.current = 10;
			unit.health.max = 10;
			unit.health.shown = unit.health.max;
			unit.cost = 9999;
			unit.moveRange = 3;
			unit.attackRange = 1;
			unit.name = "Aleksander";
			unit.attributes = ATTR_CAMPAIGN_UNIT;
			unit.src = rect(32, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_RECRUIT) {
			unit.damage = 5;
			unit.health.current = 10;
			unit.health.max = 10;
			unit.health.shown = unit.health.max;
			unit.cost = 100;
			unit.moveRange = 3;
			unit.attackRange = 1;
			unit.name = "Recruit";
			unit.attributes = ATTR_NONE;
			unit.src = rect(64, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_SLAYER) {
			unit.damage = 4;
			unit.health.current = 12;
			unit.health.max = 12;
			unit.health.shown = unit.health.max;
			unit.cost = 300;
			unit.moveRange = 3;
			unit.attackRange = 1;
			unit.name = "Slayer";
			unit.attributes = ATTR_DOUBLE_ATTACK | ATTR_PIERCING;
			unit.src = rect(96, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_MEDIC) {
			unit.damage = 2;
			unit.health.current = 8;
			unit.health.max = 8;
			unit.health.shown = unit.health.max;
			unit.cost = 500;
			unit.moveRange = 4;
			unit.attackRange = 3;
			unit.name = "Medic";
			unit.attributes = ATTR_HEALER;
			unit.src = rect(64, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_ARTILLERY) {
			unit.damage = 13;
			unit.health.current = 4;
			unit.health.max = 4;
			unit.health.shown = unit.health.max;
			unit.cost = 800;
			unit.moveRange = 3;
			unit.attackRange = 5;
			unit.name = "Artillery";
			unit.attributes = ATTR_ARTILLERY | ATTR_ARMOR;
			unit.src = rect(128, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_TANK) {
			unit.damage = 15;
			unit.health.current = 20;
			unit.health.max = 20;
			unit.health.shown = unit.health.max;
			unit.cost = 700;
			unit.moveRange = 6;
			unit.attackRange = 1;
			unit.name = "Tank";
			unit.attributes = ATTR_ARMOR;
			unit.src = rect(160, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_MEGATANK) {
			unit.damage = 25;
			unit.health.current = 30;
			unit.health.max = 30;
			unit.health.shown = unit.health.max;
			unit.cost = 2500;
			unit.moveRange = 5;
			unit.attackRange = 1;
			unit.name = "Mega Tank";
			unit.attributes = ATTR_ARMOR;
			unit.src = rect(192, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_SCOUT) {
			unit.damage = 5;
			unit.health.current = 10;
			unit.health.max = 10;
			unit.health.shown = unit.health.max;
			unit.cost = 400;
			unit.moveRange = 7;
			unit.attackRange = 1;
			unit.name = "Scout Car";
			unit.attributes = ATTR_NONE;
			unit.src = rect(224, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_BOMBER) {
			unit.damage = 25;
			unit.health.current = 15;
			unit.health.max = 15;
			unit.health.shown = unit.health.max;
			unit.cost = 1600;
			unit.moveRange = 5;
			unit.attackRange = 1;
			unit.name = "Bomber";
			unit.attributes = ATTR_FLYING;
			unit.src = rect(256, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_FIGHTER) {
			unit.damage = 20;
			unit.health.current = 15;
			unit.health.max = 15;
			unit.health.shown = unit.health.max;
			unit.cost = 1600;
			unit.moveRange = 5;
			unit.attackRange = 1;
			unit.name = "Fighter";
			unit.attributes = ATTR_FLYING;
			unit.src = rect(288, 0, TILE_SIZE, TILE_SIZE);
		}
		if (i == UNIT_TYPE_MINDSLAVE) {
			unit.damage = 3;
			unit.health.current = 5;
			unit.health.max = 5;
			unit.health.shown = unit.health.max;
			unit.cost = 150;
			unit.moveRange = 4;
			unit.attackRange = 1;
			unit.name = "Mind Slave";
			unit.attributes = ATTR_ARTILLERY;
			unit.src = rect(0, 32, TILE_SIZE, TILE_SIZE);
		}

		UNIT_TYPES[i] = unit;
	}
	
	for (u16 i = 0; i < BUILDING_TYPE_COUNT; ++i) {
		BuildingTypeData building;
		building.type = (BuildingType)i;

		if (i == BUILDING_TYPE_COMMAND_CENTER) {
			building.cost = 400;
			building.src = rect(32, 0, TILE_SIZE, TILE_SIZE);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_BUILDER]);
			building.name = "Command Center";
		}
		if (i == BUILDING_TYPE_PRODUCTION) {
			building.src = rect(128, 0, TILE_SIZE, TILE_SIZE);
			building.cost = 200;
			building.name = "Mine";
		}
		if (i == BUILDING_TYPE_BARRACKS) {
			building.src = rect(0, 0, TILE_SIZE, TILE_SIZE);
			building.cost = 500;
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_RECRUIT]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_SLAYER]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_MEDIC]);
			building.name = "Barracks";
		}
		if (i == BUILDING_TYPE_FACTORY) {
			building.src = rect(64, 0, TILE_SIZE, TILE_SIZE);
			building.cost = 1000;
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_TANK]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_MEGATANK]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_ARTILLERY]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_SCOUT]);
			building.name = "Factory";
		}
		if (i == BUILDING_TYPE_AIRPORT) {
			building.src = rect(96, 0, TILE_SIZE, TILE_SIZE);
			building.cost = 1500;
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_FIGHTER]);
			building.trainable.push_back(UNIT_TYPES[UNIT_TYPE_BOMBER]);
			building.name = "Airport";
		}

		BUILDING_TYPES[i] = building;
	}
}

Unit create_unit(UnitType type, Owner owner, u32 x, u32 y) {
	Unit unit;
	unit.active = true;
	unit.owner = owner;
	unit.x = x * TILE_SIZE;
	unit.y = y * TILE_SIZE;
	unit.type = type;

	unit.attackRange = UNIT_TYPES[type].attackRange;
	unit.damage = UNIT_TYPES[type].damage;
	unit.health = UNIT_TYPES[type].health;
	unit.moveRange = UNIT_TYPES[type].moveRange;
	unit.src = UNIT_TYPES[type].src;
	unit.attackRange = UNIT_TYPES[type].attackRange;
	unit.attributes = UNIT_TYPES[type].attributes;
	return unit;
}

Building create_building(BuildingType type, Owner owner, u32 x, u32 y) {
	Building building;
	building.active = true;
	building.owner = owner;
	building.x = x * TILE_SIZE;
	building.y = y * TILE_SIZE;
	building.type = type;

	building.src = BUILDING_TYPES[type].src;
	//building.captureBar.current = building.captureBar.max = building.captureBar.shown = 10.0f;

	return building;
}

UnitTypeData get_type_data(UnitType type) {
	if (type == UNIT_TYPE_NONE) return { 0 };
	return UNIT_TYPES[type];
}

BuildingTypeData get_type_data(BuildingType type) {
	if (type == BUILDING_TYPE_NONE) return { 0 };
	return BUILDING_TYPES[type];
}

Faction get_faction_from_type(FactionType type) {
	Faction faction;
	faction.type = type;
	if (type == FACTION_REBELS) {
		//faction.units.push_back(get_type_data((UnitType)UNIT_TYPE_ELEANOR));
		//faction.units.push_back(get_type_data((UnitType)UNIT_TYPE_ALEKSANDER_BONES));
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_RECRUIT) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_SLAYER) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_ARTILLERY) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_TANK) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_MEGATANK) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_SCOUT) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_BOMBER) );
		faction.units.push_back( get_type_data((UnitType)UNIT_TYPE_FIGHTER) );

		faction.buildings.push_back( get_type_data((BuildingType)BUILDING_TYPE_COMMAND_CENTER) );
		faction.buildings.push_back( get_type_data((BuildingType)BUILDING_TYPE_BARRACKS) );
		faction.buildings.push_back( get_type_data((BuildingType)BUILDING_TYPE_FACTORY) );
		faction.buildings.push_back( get_type_data((BuildingType)BUILDING_TYPE_AIRPORT) );
		faction.buildings.push_back( get_type_data((BuildingType)BUILDING_TYPE_PRODUCTION) );
		faction.name = "Rebels";
	}
	if (type == FACTION_MONSTERS) {
		faction.name = "Monsters";
	}
	if (type == FACTION_GLOBALIST) {
		faction.units.push_back(get_type_data((UnitType)UNIT_TYPE_MINDSLAVE));
		faction.name = "Globalists";
	}
	return faction;
}