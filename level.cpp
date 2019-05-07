#include "level.h"
#include "globals.h"
#include <SOIL.h>
#include <defines.h>
#include <maths.h>
#include <fstream>
#include <string>

#define ANIMATION_SPEED 60

//INTERNAL functions
internal inline
int attack_heuristic(Level* level, Unit* unit, int x, int y) {
	return 1;
}

internal inline
int move_heuristic(Level* level, Unit* unit, int x, int y) {
	int tileID = level->map.grid[x + y * level->map.width];
	for (u16 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
		Player* current = &level->players[currPlayer];
		for (u16 i = 0; i < current->units.size(); ++i) {
			Unit* currentUnit = &current->units[i];
			vec2 unit_pos = V2(currentUnit->x / TILE_SIZE, currentUnit->y / TILE_SIZE);
			vec2 tile_pos = V2(x, y);
			if (tile_pos == unit_pos) {
				return -1;
			}
		}
	}
	if (tileID == 1) {
		return 2;
	}
	if (tileID == 12) {
		if (unit->attributes & ATTR_FLYING)
			return 1;
		else
			return -1;
	}
	return 1;
}

internal inline
bool all_units_taken_turn(std::vector<Unit>& units) {
	int num_taken_turn = 0;
	for (u16 i = 0; i < units.size(); ++i)
		if (units[i].active == false) {
			num_taken_turn++;
		}
	return (num_taken_turn == units.size());
}

internal inline
void draw_health_bar(int xPos, int yPos, Player* player, f32 health, f32 max_health, bool isHealthDeclining, Texture healthBar) {
	int padding = 1;
	int max_width = 24;
	int height = 2;
	if (isHealthDeclining) {
		max_width = 30;
		height = 4;
		padding = 2;
	}
	if (max_health == 0) BMT_LOG(FATAL_ERROR, "Max health is 0. Divide by zero error.");
	if (health > max_health) {
		BMT_LOG(WARNING, "health (%f) is higher than max_health (%f)", health, max_health);
		health = max_health;
	}
	float width = (float)((float)health / (float)max_health) * max_width;

	xPos += (TILE_SIZE / 2) - (max_width / 2);

	draw_rectangle(xPos - padding, yPos - padding, max_width + (padding * 2), height + (padding * 2), BLACK);
	Rect source = rect(0, 0, healthBar.width, healthBar.height);
	Rect dest = rect(xPos, yPos, width, height);

	draw_texture_EX(healthBar, source, dest, player->color);
}

internal inline
int add_cost_from_parents(NodePtr node) {
	int cost = 0;
	while (node->parent != NULL) {
		cost += node->fCost;
		node = node->parent;
	}
	return cost;
}

//TODO: Rewrite to be more efficient. Right now it is very brute-force.
internal inline
std::vector<vec2> find_range(Level* level, int range, Unit* unit, int(*heuristic)(Level*, Unit*, int, int)) {
	std::vector<NodePtr> tiles;
	std::vector<vec2> tilesInRange;
	Map& map = level->map;

	NodePtr current(new Node(unit->x / (TILE_SIZE), unit->y / (TILE_SIZE), NULL, 0, 0));
	tiles.push_back(current);
	tilesInRange.push_back(V2(unit->x / (TILE_SIZE), unit->y / (TILE_SIZE)));

	while (!tiles.empty()) {
		current = tiles.back();
		tiles.pop_back();
		if (add_cost_from_parents(current) < range) {

			int nextX = current->x - 1;
			int nextY = current->y;
			int heuristicCost = heuristic(level, unit, nextX, nextY);
			if (nextX >= 0 && nextY >= 0 && nextX < map.width && nextY < map.height && heuristicCost > 0) {
				NodePtr node(new Node(nextX, nextY, current, heuristicCost, 0));
				tiles.push_back(node);

				bool hasVisited = false;
				for (int i = 0; i < tilesInRange.size(); ++i) {
					if (nextX == tilesInRange[i].x && nextY == tilesInRange[i].y)
						hasVisited = true;
				}
				if (!hasVisited)
					tilesInRange.push_back(V2(nextX, nextY));
			}

			nextX = current->x + 1;
			nextY = current->y;
			heuristicCost = heuristic(level, unit, nextX, nextY);
			if (nextX >= 0 && nextY >= 0 && nextX < map.width && nextY < map.height && heuristicCost > 0) {
				NodePtr node(new Node(nextX, nextY, current, heuristicCost, 0));
				tiles.push_back(node);

				bool hasVisited = false;
				for (int i = 0; i < tilesInRange.size(); ++i) {
					if (nextX == tilesInRange[i].x && nextY == tilesInRange[i].y)
						hasVisited = true;
				}
				if (!hasVisited)
					tilesInRange.push_back(V2(nextX, nextY));
			}

			nextX = current->x;
			nextY = current->y - 1;
			heuristicCost = heuristic(level, unit, nextX, nextY);
			if (nextX >= 0 && nextY >= 0 && nextX < map.width && nextY < map.height && heuristicCost > 0) {
				NodePtr node(new Node(nextX, nextY, current, heuristicCost, 0));
				tiles.push_back(node);

				bool hasVisited = false;
				for (int i = 0; i < tilesInRange.size(); ++i) {
					if (nextX == tilesInRange[i].x && nextY == tilesInRange[i].y)
						hasVisited = true;
				}
				if (!hasVisited)
					tilesInRange.push_back(V2(nextX, nextY));
			}

			nextX = current->x;
			nextY = current->y + 1;
			heuristicCost = heuristic(level, unit, nextX, nextY);
			if (nextX >= 0 && nextY >= 0 && nextX < map.width && nextY < map.height && heuristicCost > 0) {
				NodePtr node(new Node(nextX, nextY, current, heuristicCost, 0));
				tiles.push_back(node);

				bool hasVisited = false;
				for (int i = 0; i < tilesInRange.size(); ++i) {
					if (nextX == tilesInRange[i].x && nextY == tilesInRange[i].y)
						hasVisited = true;
				}
				if (!hasVisited)
					tilesInRange.push_back(V2(nextX, nextY));
			}

		}
	}

	return tilesInRange;
}

internal inline
bool compare_ptr_to_node(NodePtr a, NodePtr b) {
	return a->fCost > b->fCost;
}

internal inline
void process_successor(int x, int y, int fCost, NodePtr parent, std::vector<NodePtr>& open, std::vector<NodePtr>& closed) {
	if (x < 0 || y < 0) return;

	//search the open list for a tile with a better cost 
	for (int i = 0; i < open.size(); ++i) {
		int nodeX = open[i]->x;
		int nodeY = open[i]->y;

		if (nodeX == x && nodeY == y) {
			if (open[i]->fCost <= fCost)
				return; //skip because it's a worse tile
			break; //break out because it's a better tile
		}
	}

	//closed list
	for (int i = 0; i < closed.size(); ++i) {
		int nodeX = closed[i]->x;
		int nodeY = closed[i]->y;

		if (nodeX == x && nodeY == y) {
			if (closed[i]->fCost <= fCost)
				return; //skip because it's a worse tile.
			break; //break out because it's a better tile
		}
	}
	open.push_back(NodePtr(new Node(x, y, parent, fCost, 0)));
}

internal inline
inline std::vector<vec2> reconstruct_path(NodePtr current) {
	std::vector<vec2> path;

	while (current->parent != NULL) {
		path.push_back(V2(current->x, current->y));
		current = current->parent;
	}
	path.push_back(V2(current->x, current->y));
	return path;
}

internal inline
std::vector<vec2> pathfind(Level* level, Unit* unit, int(*heuristic)(Level*, Unit*, int, int), vec2 start, vec2 dest) {
	if (start.x < 0 || start.y < 0 || start.x > level->map.width - 1 || start.y > level->map.height - 1)
		BMT_LOG(FATAL_ERROR, "vec2 start was out of bounds. start = (%f, %f)", start.x, start.y);
	if (dest.x < 0 || dest.y < 0 || dest.x > level->map.width - 1 || dest.y > level->map.height - 1)
		BMT_LOG(FATAL_ERROR, "vec2 dest was out of bounds. dest = (%f, %f)", dest.x, dest.y);

	std::vector<NodePtr> open;
	std::vector<NodePtr> closed;
	NodePtr current;
	open.push_back(NodePtr(new Node(start.x, start.y, NULL, 0, 0)));

	u16 tries = 0;
	while (!open.empty() && tries < MAX_PATHFIND_TRIES) {
		tries++;

		std::sort(open.begin(), open.end(), compare_ptr_to_node);
		current = open.back();
		open.pop_back();

		if (current->x == dest.x && current->y == dest.y) {
			return reconstruct_path(current);
		}

		//if heuristic cost = -1 then the tile is blocked and cant be move to.
		int x = current->x - 1;
		int y = current->y;
		if (x > level->map.width) continue;
		if (y > level->map.height) continue;
		int heuristicCost = heuristic(level, unit, x, y);
		if (heuristicCost != -1)
			process_successor(x, y, current->gCost + heuristicCost + getDistanceM(x, y, dest.x, dest.y), current, open, closed);
		else if (dest.x == x && dest.y == y)
			return reconstruct_path(current);

		x = current->x + 1;
		y = current->y;
		if (x > level->map.width) continue;
		if (y > level->map.height) continue;
		heuristicCost = heuristic(level, unit, x, y);
		if (heuristicCost != -1)
			process_successor(x, y, current->gCost + heuristicCost + getDistanceM(x, y, dest.x, dest.y), current, open, closed);
		else if (dest.x == x && dest.y == y)
			return reconstruct_path(current);

		x = current->x;
		y = current->y - 1;
		if (x > level->map.width) continue;
		if (y > level->map.height) continue;
		heuristicCost = heuristic(level, unit, x, y);
		if (heuristicCost != -1)
			process_successor(x, y, current->gCost + heuristicCost + getDistanceM(x, y, dest.x, dest.y), current, open, closed);
		else if (dest.x == x && dest.y == y)
			return reconstruct_path(current);

		x = current->x;
		y = current->y + 1;
		if (x > level->map.width) continue;
		if (y > level->map.height) continue;
		heuristicCost = heuristic(level, unit, x, y);
		if (heuristicCost != -1)
			process_successor(x, y, current->gCost + heuristicCost + getDistanceM(x, y, dest.x, dest.y), current, open, closed);
		else if (dest.x == x && dest.y == y)
			return reconstruct_path(current);

		closed.push_back(current);
	}
	if (tries == MAX_PATHFIND_TRIES) {
		return reconstruct_path(current);
	}
	std::vector<vec2> blank;
	blank.push_back(start);
	return blank;
}

internal inline
void draw_tiles(Level& level, std::vector<vec2> tiles, float r, float g, float b, float a) {
	for (u16 i = 0; i < tiles.size(); ++i) {
		vec2 tile_pos = tiles[i];
		draw_rectangle((tile_pos.x * TILE_SIZE) + level.xPos, (tile_pos.y * TILE_SIZE) + level.yPos, TILE_SIZE, TILE_SIZE, r, g, b, a);
	}
}

//returns true if the unit has finished pathing, otherwise false.
internal inline
bool move_unit(Level* level, Unit* unit, bool cameraFollow) {
	if (unit->path.size() == 0)
		return true;
	vec2 moveTo = V2(unit->path.back().x * TILE_SIZE, unit->path.back().y * TILE_SIZE);

	if (unit->x < moveTo.x) {
		unit->x += MOVE_SPEED;
		if (cameraFollow)
			level->xPos -= MOVE_SPEED;
	}
	if (unit->x > moveTo.x) {
		unit->x -= MOVE_SPEED;
		if (cameraFollow)
			level->xPos += MOVE_SPEED;
	}
	if (unit->y < moveTo.y) {
		unit->y += MOVE_SPEED;
		if (cameraFollow)
			level->yPos -= MOVE_SPEED;
	}
	if (unit->y > moveTo.y) {
		unit->y -= MOVE_SPEED;
		if (cameraFollow)
			level->yPos += MOVE_SPEED;
	}
	if (unit->x == moveTo.x && unit->y == moveTo.y) {
		unit->path.pop_back();
		if (unit->path.size() == 0) {
			return true;
		}
	}
	return false;
}

internal inline
Unit* get_closest_enemy(Level& level, Unit* source) {
	f32 lowest_distance = FLT_MAX;
	Unit* closestEnemy = NULL;

	for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
		if (currPlayer == source->owner) continue;
		Player* current = &level.players[currPlayer];
		if (current->allied[source->owner]) continue;

		for (u16 i = 0; i < current->units.size(); ++i) {
			Unit* unit = &current->units[i];
			f32 dist = getDistanceE((f32)source->x, (f32)source->y, (f32)unit->x, (f32)unit->y);
			if (dist < lowest_distance) {
				lowest_distance = dist;
				closestEnemy = unit;
			}
		}
	}

	if (closestEnemy == NULL || lowest_distance == FLT_MAX) return NULL;
	return closestEnemy;
}

internal inline
u16 get_tile_damage_reduction(Level& level, int tile_x, int tile_y) {
	//tile #1 is a forest, with 2 damage reduction.
	if (level.map.grid[tile_x + tile_y * level.map.width] == 1) {
		return 2;
	}
	return 0;
}

internal inline
void attack_unit(Level& level, LevelScene* scene, Unit* attacker, Unit* defender) {
	u16 reduction = get_tile_damage_reduction(level, defender->x / TILE_SIZE, defender->y / TILE_SIZE);

	defender->health.current -= (attacker->damage - reduction);

	if (defender->health.current > defender->health.max)
		defender->health.current = defender->health.max;

	if (level.unitIsAttacked != NULL)
		level.unitIsAttacked(level, scene, defender, defender->x / TILE_SIZE, defender->y / TILE_SIZE);

	if (defender->health.current <= 0) {
		if (level.unitDies != NULL)
			level.unitDies(level, scene, defender, defender->x / TILE_SIZE, defender->y / TILE_SIZE);
	}

	//if melee attack, play melee animation
	//if (attacker->attackRange == 1) {
	//	if (attacker->x < defender->x) {
	//		level.attackAnim.row = 0;
	//		start_animation(level.attackAnim, false, 0);
	//		level.attackAnimPos.x = attacker->x + (TILE_SIZE / 2);
	//		level.attackAnimPos.y = attacker->y;
	//	}
	//	else if (attacker->x > defender->x) {
	//		level.attackAnim.row = 0;
	//		start_animation(level.attackAnim, false, FLIP_HORIZONTAL);
	//		level.attackAnimPos.x = attacker->x - (TILE_SIZE / 2);
	//		level.attackAnimPos.y = attacker->y;
	//	}
	//	else if (attacker->y < defender->y) {
	//		level.attackAnim.row = 1;
	//		start_animation(level.attackAnim, false, 0);
	//		level.attackAnimPos.x = attacker->x;
	//		level.attackAnimPos.y = attacker->y + (TILE_SIZE / 2);
	//	}
	//	else if (attacker->y > defender->y) {
	//		level.attackAnim.row = 2;
	//		start_animation(level.attackAnim, false, 0);
	//		level.attackAnimPos.x = attacker->x;
	//		level.attackAnimPos.y = attacker->y - (TILE_SIZE / 2);
	//	}
	//}
	//else {
	//	//play ranged animation
	//}
}

internal inline
inline bool out_of_bounds(Level& level, vec2 pos) {
	return (pos.x < 0 || pos.y < 0 || pos.x > level.map.width - 1 || pos.y > level.map.height - 1);
}

internal inline
bool is_in_range(Unit* unit, std::vector<vec2> tiles) {
	for (u16 j = 0; j < tiles.size(); ++j) {
		if (V2(unit->x / TILE_SIZE, unit->y / TILE_SIZE) == tiles[j])
			return true;
	}
	return false;
}

internal inline
bool is_in_range(vec2 pos, std::vector<vec2> tiles) {
	for (u16 j = 0; j < tiles.size(); ++j) {
		if (V2(pos.x, pos.y) == tiles[j])
			return true;
	}
	return false;
}

internal inline
inline bool value_is_close_to(f32 value, f32 compare, f32 bias) {
	return (value > compare - bias && value < compare + bias);
}

internal inline
inline Unit* get_target(Level& level, Unit* src) {
	return get_closest_enemy(level, src);
}

internal inline
void activate_building_effects(Level& level) {
	Player* currPlayer = &level.players[level.currentTurn];

	for (u16 currBuilding = 0; currBuilding < currPlayer->buildings.size(); ++currBuilding) {
		BuildingType bt = currPlayer->buildings[currBuilding].type;
	}

	//increase current players money based on their money production buildings
	for (u16 currBuilding = 0; currBuilding < currPlayer->buildings.size(); ++currBuilding) {
		BuildingType bt = currPlayer->buildings[currBuilding].type;
		currPlayer->buildings[currBuilding].active = true;
		if (bt == BUILDING_TYPE_PRODUCTION) {
			currPlayer->money += PRODUCTION_BUILDING_MONEY_GAIN;
		}
		
		//heal all units standing on buildings
		for (u16 currUnit = 0; currUnit < currPlayer->units.size(); ++currUnit) {
			Unit* unit = &currPlayer->units.at(currUnit);
			Building* building = &currPlayer->buildings.at(currBuilding);
			if (unit->x == building->x && unit->y == building->y) {
				currPlayer->units[currUnit].health.current += BUILDING_HEAL_AMOUNT;
				if (currPlayer->units[currUnit].health.current > currPlayer->units[currUnit].health.max) {
					currPlayer->units[currUnit].health.current = currPlayer->units[currUnit].health.max;
				}
			}
		}
	}
	if (currPlayer->money > MAX_MONEY)
		currPlayer->money = MAX_MONEY;

	for (u16 currUnit = 0; currUnit < currPlayer->units.size(); ++currUnit) {
		currPlayer->units[currUnit].active = true;
	}
}

internal inline
bool ai_should_attack(Level& level, Player& player) {
	return true;
}

internal inline
void set_ai_path(Level& level, Unit* unit, vec2 dest) {
	unit->path = pathfind(
		&level,
		unit,
		move_heuristic,
		V2(unit->x / TILE_SIZE, unit->y / TILE_SIZE),
		dest
	);
	//trim path to cut off tiles that are further away than the units move range
	std::vector<vec2> moveableTiles = find_range(&level, unit->moveRange, unit, move_heuristic);
	int cost = 0;
	for (int j = unit->path.size() - 1; j >= 0; --j) {
		cost += move_heuristic(&level, unit, unit->path.at(j).x, unit->path.at(j).y);
		if (cost >= unit->moveRange) {
			unit->path.erase(unit->path.begin() + j);
		}
	}
}

internal inline
i32 num_building_count(Player& player, BuildingType type) {
	i32 count = 0;
	for (i32 i = 0; i < player.buildings.size(); ++i)
		if (player.buildings[i].type == type)
			count++;
	return count;
}

internal inline
i32 num_unit_count(Player& player, UnitType type) {
	i32 count = 0;
	for (i32 i = 0; i < player.units.size(); ++i)
		if (player.units[i].type == type) 
			count++;
	return count;
}

internal inline
BuildingType get_home_building(UnitType type) {
	switch (type) {
	case UNIT_TYPE_RECRUIT:
	case UNIT_TYPE_SLAYER:
	case UNIT_TYPE_MEDIC:
		return BUILDING_TYPE_BARRACKS;
	case UNIT_TYPE_TANK:
	case UNIT_TYPE_MEGATANK:
	case UNIT_TYPE_ARTILLERY:
	case UNIT_TYPE_SCOUT:
		return BUILDING_TYPE_FACTORY;
	case UNIT_TYPE_FIGHTER:
	case UNIT_TYPE_BOMBER:
		return BUILDING_TYPE_AIRPORT;
	}
	return BUILDING_TYPE_NONE;
}

internal inline
void add_unit(Player& player, std::vector<UnitType>& units, UnitType type) {
	BuildingType source = get_home_building(type);
	bool sourceBuildingExists = false;
	for (u32 i = 0; i < player.buildings.size(); ++i) {
		if (player.buildings[i].type == source && player.buildings[i].active) {
			sourceBuildingExists = true;
			player.buildings[i].active = false;
		}
	}

	if (player.money >= get_type_data(type).cost && sourceBuildingExists) {
		player.money -= get_type_data(type).cost;
		units.push_back(type);
	}
}

internal inline
std::vector<UnitType> get_next_units(Player& player) {
	std::vector<UnitType> units;
	switch (player.faction.type) {
	case FACTION_REBELS:
		if (player.personality == PERSONALITY_BALANCED) {
			if (num_unit_count(player, UNIT_TYPE_BUILDER) < 1)
				add_unit(player, units, UNIT_TYPE_BUILDER);

			if (num_unit_count(player, UNIT_TYPE_RECRUIT) < 3)
				add_unit(player, units, UNIT_TYPE_RECRUIT);
	
			if (num_unit_count(player, UNIT_TYPE_SLAYER) < 1)
				add_unit(player, units, UNIT_TYPE_SLAYER);

			if (num_unit_count(player, UNIT_TYPE_TANK) < 2)
				add_unit(player, units, UNIT_TYPE_TANK);

			if(num_unit_count(player, UNIT_TYPE_MEGATANK) < 1)
				add_unit(player, units, UNIT_TYPE_MEGATANK);

			if (num_unit_count(player, UNIT_TYPE_SCOUT) < 2)
				add_unit(player, units, UNIT_TYPE_SCOUT);
		}
		break;
	case FACTION_GLOBALIST:

		break;
	}
	return units;
}

internal inline
BuildingType get_next_building(Player& player) {
	switch (player.faction.type) {
	case FACTION_REBELS:
		if (player.personality == PERSONALITY_BALANCED) {
			if (num_building_count(player, BUILDING_TYPE_COMMAND_CENTER) < 1)
				return BUILDING_TYPE_COMMAND_CENTER;

			if (num_building_count(player, BUILDING_TYPE_PRODUCTION) < 3)
				return BUILDING_TYPE_PRODUCTION;

			if (num_building_count(player, BUILDING_TYPE_BARRACKS) < 2)
				return BUILDING_TYPE_BARRACKS;

			if (num_building_count(player, BUILDING_TYPE_PRODUCTION) < 6)
				return BUILDING_TYPE_PRODUCTION;

			if (num_building_count(player, BUILDING_TYPE_FACTORY) < 1)
				return BUILDING_TYPE_FACTORY;

			if (num_building_count(player, BUILDING_TYPE_AIRPORT) < 1)
				return BUILDING_TYPE_AIRPORT;
		}
		break;
	case FACTION_GLOBALIST:

		break;
	}
	return BUILDING_TYPE_NONE;
}

internal inline
bool building_on_tile(Level& level, vec2 tile) {
	for (i32 i = 0; i < OWNER_COUNT; ++i) {
		Player* current = &level.players[i];
		for (i32 j = 0; j < current->buildings.size(); ++j) {
			if (current->buildings[j].x / TILE_SIZE == tile.x && current->buildings[j].y / TILE_SIZE == tile.y)
				return true;
		}
	}
	return false;
}

internal inline
bool can_build(Level& level, i32 tileX, i32 tileY) {
	i32 tileID = level.map.grid[tileX + tileY * level.map.width];
	switch (tileID) {
	case 1:
		return false;
	default:
		return true;
	}

	return false;
}

internal inline
bool is_production_building(BuildingType type) {
	switch (type) {
	case BUILDING_TYPE_AIRPORT:
	case BUILDING_TYPE_BARRACKS:
	case BUILDING_TYPE_COMMAND_CENTER:
	case BUILDING_TYPE_FACTORY:
		return true;
		break;
	default:
		return false;
	}
	return false;
}

internal inline
void take_ai_turn(Level& level, Player& player, LevelScene* scene) {
	Unit* unit = &player.units[player.lastSelectedUnit];
	bool shouldAttack = ai_should_attack(level, player);

	if (unit->active) {
		unit->active = false;
		if(unit->attributes & ATTR_BUILDER) {
			vec2 CCPos;
			for (u32 i = 0; i < player.buildings.size(); ++i)
				if (player.buildings[i].type == BUILDING_TYPE_COMMAND_CENTER)
					CCPos = V2(player.buildings[i].x, player.buildings[i].y);
			CCPos.x /= TILE_SIZE;
			CCPos.y /= TILE_SIZE;

			std::vector<vec2> range = find_range(&level, unit->moveRange, unit, move_heuristic);
			i32 rand = random_int(0, range.size() - 1);
			vec2 tile = range.at(rand);
			while (true) {
				if (!building_on_tile(level, tile) && getDistanceM(tile.x, tile.y, CCPos.x, CCPos.y) < 6 && can_build(level, tile.x, tile.y))
					break;
				tile = range.at(rand);
				rand = random_int(0, range.size() - 1);
			}
			set_ai_path(level, unit, tile);
		}
		else {
			//TODO: Have target chosen based off more than just closeness. Should also take into account things like
			//HP and how dangerous that unit could be.
			Unit* target = get_target(level, unit);
			if (target == NULL) {
				BMT_LOG(WARNING, "Target was NULL, no closest enemy (enemies most likely no longer exist!)");
				return;
			}
			set_ai_path(level, unit, V2(target->x / TILE_SIZE, target->y / TILE_SIZE));
		}
		//center camera on unit
		level.xPos = -unit->x + (get_virtual_width() / 2);
		level.yPos = -unit->y + (get_virtual_height() / 2);
	}
	else {
		if (move_unit(&level, &player.units[player.lastSelectedUnit], true)) {
			level.sleepUpdateTime = 30;

			if (unit->attributes & ATTR_BUILDER) {
				BuildingType toBuild = get_next_building(player);
				if(toBuild != BUILDING_TYPE_NONE)
 					spawn_building(level, toBuild, unit->owner, unit->x / TILE_SIZE, unit->y / TILE_SIZE);
			}
			else {
				//attack target if in range
				std::vector<vec2> moveable_tiles = find_range(&level, unit->attackRange, unit, attack_heuristic);
				for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
					Player* current = &level.players[currPlayer];
					if (currPlayer == (level.currentTurn))
						continue;

					if (current->allied[player.units[player.lastSelectedUnit].owner])
						continue;

					for (u16 j = 0; j < current->units.size(); ++j) {
						if (current->units[j].owner != unit->owner) {
							bool isInRange = is_in_range(&current->units[j], moveable_tiles);

							if (isInRange) {

								attack_unit(level, scene, unit, &current->units[j]);
								level.sleepUpdateTime = 50;
								//double loop break
								goto endturn;
							}
						} 
					} //loop units
				} //loop players
			} //if not builder
		endturn:

			player.lastSelectedUnit++;
			//if units have all taken turn, increase turn.
			if (player.lastSelectedUnit == player.units.size()) {
				player.lastSelectedUnit = 0;
				level.nextTurnSplash.xPos = 0;
				level.nextTurnSplash.yPos = (get_virtual_height() / 2) - (scene->nextTurn.height / 2);
				level.nextTurnSplash.begin(scene->nextTurn, SPLASH_FADE_SPEED);

				level.state = STATE_SPLASH;

				//train a unit if the AI has the money
				std::vector<UnitType> toTrain = get_next_units(player);
				for (u32 i = 0; i < player.buildings.size(); ++i)
					player.buildings[i].active = true;
				for (u32 i = 0; i < toTrain.size(); ++i) {
					BuildingType source = get_home_building(toTrain[i]);
					for (u32 j = 0; j < player.buildings.size(); ++j) {
						if (player.buildings[j].type == source && player.buildings[j].active) {
							player.buildings[j].active = false;
							spawn_unit(level, toTrain[i], (Owner)level.currentTurn, player.buildings[j].x / TILE_SIZE, player.buildings[j].y / TILE_SIZE);
						}
					}
				}

				activate_building_effects(level);
				return;
			}
		}

	}
}

internal inline
void update_health(HealthBar& health, Texture healthBar, Player* player, f32 xPos, f32 yPos) {
	bool healthDeclining = (health.shown > health.current);
	bool healthIncreasing = (health.shown < health.current);
	draw_health_bar(xPos, yPos, player, health.shown, health.max, healthDeclining || healthIncreasing, healthBar);

	if (healthDeclining) {
		health.shown -= 0.1f;
	}
	if (healthIncreasing) {
		health.shown += 0.1f;
	}
	if (value_is_close_to(health.shown, health.current, .2f)) {
		health.shown = health.current;
	}
}

internal inline
Map create_map(int width, int height) {
	Map map;
	map.width = width;
	map.height = height;
	map.grid = new int[width * height];
	for (u32 i = 0; i < width * height; ++i) {
		map.grid[i] = 0;
	}
	return map;
}

internal inline
inline void draw_map(Map& map, int xPos, int yPos) {
#define PADDING 3
	int x0 = (-xPos / (TILE_SIZE));
	int x1 = (-xPos / (TILE_SIZE)) + (get_virtual_width() / TILE_SIZE) + PADDING;
	int y0 = (-yPos / (TILE_SIZE));
	int y1 = (-yPos / (TILE_SIZE)) + (get_virtual_height() / TILE_SIZE) + PADDING;
	clamp<int>(x0, 0, map.width);
	clamp<int>(y0, 0, map.height);
	clamp<int>(x1, 0, map.width);
	clamp<int>(y1, 0, map.height);
#undef PADDING

	Rect dest;
	dest.width = TILE_SIZE;
	dest.height = TILE_SIZE;
	Rect src;
	src.width = TILE_SIZE;
	src.height = TILE_SIZE;
	int num_tiles_across = map.tilesheet1.width / TILE_SIZE;
	if (num_tiles_across == 0) num_tiles_across = 1;
	for (int y = y0; y < y1; ++y) {
		for (int x = x0; x < x1; ++x) {
			dest.x = (x * (TILE_SIZE)) + xPos;
			dest.y = (y * (TILE_SIZE)) + yPos;
			src.x = map.grid[x + y * map.width] % num_tiles_across;
			src.y = map.grid[x + y * map.width] / num_tiles_across;
			src.x *= TILE_SIZE;
			src.y *= TILE_SIZE;
			draw_texture_EX((x + y) % 2 == 0 ? map.tilesheet1 : map.tilesheet2, src, dest);
		}
	}
}

internal inline
void dispose_map(Map& map) {
	delete[] map.grid;
}

Level load_level(const char* path, Texture tilesheetOne, Texture tilesheetTwo) {
	Level level = { 0 };
	level.currentDay = 1;
	level.menu.width = 2;
	level.menu.padding = 3;
	level.menu.xPos = 130;
	level.menu.yPos = 10;

	std::ifstream file;
	file.open(path);

	file >> level.map.width;
	file.ignore(100, '\n');
	file >> level.map.height;
	file.ignore(100, '\n');

	level.map = create_map(level.map.width, level.map.height);
	level.map.tilesheet1 = tilesheetOne;
	level.map.tilesheet2 = tilesheetTwo;

	for (u32 y = 0; y < level.map.height; ++y) {
		for (u32 x = 0; x < level.map.width; ++x) {
			file >> level.map.grid[x + y * level.map.width];
		}
		file.ignore(100, '\n');
	}
	file.ignore(100, '\n');

	i32 size;
	file >> size;
	file.ignore(100, '\n');
	for (u16 i = 0; i < size; ++i) {
		i32 type;
		i32 owner;
		i32 x;
		i32 y;
		file >> type;
		file >> owner;
		file >> x;
		file >> y;
		Unit unit = create_unit((UnitType)type, (Owner)owner, x, y);
		level.players[owner].units.push_back(unit);
		file.ignore(100, '\n');
	}
	file.ignore(100, '\n');

	file >> size;
	file.ignore(100, '\n');
	for (u16 i = 0; i < size; ++i) {
		i32 type;
		i32 owner;
		i32 x;
		i32 y;
		file >> type;
		file >> owner;
		file >> x;
		file >> y;
		Building building = create_building((BuildingType)type, (Owner)owner, x, y);
		level.players[owner].buildings.push_back(building);
		file.ignore(100, '\n');
	}
	file.ignore(100, '\n');

	for (u16 i = 0; i < OWNER_COUNT; ++i) {
		i32 AI;
		i32 faction;
		i32 numAllied;
		file >> AI;
		file >> faction;
		file >> numAllied;

		for (u16 j = 0; j < OWNER_COUNT; ++j) {
			level.players[i].allied[j] = false;
		}

		for (u16 j = 0; j < numAllied; ++j) {
			i32 allyIndex;
			file >> allyIndex;
			level.players[i].allied[allyIndex] = true;
		}

		level.players[i].AI = AI == 0 ? false : true;
		level.players[i].faction = get_faction_from_type((FactionType)faction);
		level.players[i].personality = PERSONALITY_BALANCED;
		level.players[i].lastSelectedUnit = 0;
		file.ignore(100, '\n');
	}

	i32 goesFirst;
	file >> goesFirst;
	level.currentTurn = goesFirst;
	if (level.players[goesFirst].AI) {
		level.state = STATE_AI;
	}
	else {
		level.state = STATE_IDLE;
	}

	level.players[0].color = V4(40, 255, 40, 255);
	level.players[1].color = V4(255, 40, 40, 255);
	level.players[2].color = V4(40, 40, 255, 255);
	level.players[3].color = V4(255, 255, 40, 255);
	level.players[4].color = V4(255, 255, 255, 255);

	file.close();

	return level;
}

void save_level(const char* path) {

}

//NODE
Node::Node(int x, int y, NodePtr parent, double gCost, double hCost) {
	this->x = x;
	this->y = y;
	this->parent = parent;
	this->gCost = gCost;
	this->hCost = hCost;
	this->fCost = gCost + hCost;
}
Node::Node() {}

void push_cutscene_action(Level* level, Action* action) {
	level->actions.push_back(action);
}

void begin_cutscene(Level* level) {
	level->state = STATE_CUTSCENE;
}

//returns whether or not to skip the frame. (Whenever there is a state change, usually
//	it will skip the frame so that only one state change can happen per frame.)
internal inline
bool update_level(Level& level, vec2 mouse_pos, LevelScene* scene) {
	if (level.state != STATE_AI && level.state != STATE_CUTSCENE) {
		if (is_key_down(KEY_LEFT))
			level.xPos += SCROLL_SPEED;
		if (is_key_down(KEY_RIGHT))
			level.xPos -= SCROLL_SPEED;
		if (is_key_down(KEY_DOWN))
			level.yPos -= SCROLL_SPEED;
		if (is_key_down(KEY_UP))
			level.yPos += SCROLL_SPEED;
	}

	Unit* hoveredUnit = NULL;
	//loop players
	for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
		Player* current = &level.players[currPlayer];

		//loop player's units
		for (u16 i = 0; i < current->units.size(); ++i) {

			Unit* unit = &current->units[i];
			//if unit is clicked
			bool unitColliding = colliding(rect(unit->x + level.xPos, unit->y + level.yPos, TILE_SIZE, TILE_SIZE), mouse_pos.x, mouse_pos.y);
			if (unitColliding) hoveredUnit = unit;
			if (unitColliding && is_button_released(MOUSE_BUTTON_LEFT)) {

				//if player isn't an AI and it is the player's turn.
				if (!current->AI && currPlayer == (level.currentTurn)) {
					//if idle, switch to move state and find the clicked unit's move range
					if (level.state == STATE_IDLE) {
						if (unit->active) {
							//play_sound(button_press_sound);

							level.selectedUnit = unit;
							level.move_tiles = find_range(&level, unit->moveRange, unit, move_heuristic);
							level.state = STATE_MOVESTATE;
							level.menu.xPos = unit->x + (TILE_SIZE / 2);
							level.menu.yPos = unit->y;
							return true;
						}
					}
				} // end if(player isn't AI and player's turn)

				//if attacking, deal damage to the unit that was clicked on.
				if (level.state == STATE_ATTACKING) {
					if (level.selectedUnit == NULL)
						BMT_LOG(FATAL_ERROR, "Somehow the selected unit became unselected while attacking.");

					bool isInRange = is_in_range(unit, level.attack_tiles);
					Player* selectedUnitOwner = &level.players[level.selectedUnit->owner];

					//if clicked unit isn't allied to selected unit and if clicked unit doesn't have the same owner as selected unit and is in range
					if (!selectedUnitOwner->allied[unit->owner] && unit->owner != (level.currentTurn) && isInRange) {
						attack_unit(level, scene, level.selectedUnit, unit);

						level.selectedUnit->active = false;
						level.state = STATE_IDLE;
						if (level.unitEndsTurn != NULL)
							level.unitEndsTurn(level, scene, level.selectedUnit, level.selectedUnit->x / TILE_SIZE, level.selectedUnit->y / TILE_SIZE);
						return true;
					}
				} // end attack state
				//if healing, heal the unit that was clicked on
				if (level.state == STATE_HEALING) {
					if (level.selectedUnit == NULL)
						BMT_LOG(FATAL_ERROR, "Somehow the selected unit became unselected while healing");

					bool isInRange = is_in_range(unit, level.attack_tiles);
					Player* selectedUnitOwner = &level.players[level.selectedUnit->owner];

					//if clicked unit isn't allied to selected unit and if clicked unit doesn't have the same owner as selected unit and is in range
					if (selectedUnitOwner->allied[unit->owner] || unit->owner == (level.currentTurn)) {
						if (isInRange) {
							unit->health.current += level.selectedUnit->damage;

							if (unit->health.current > unit->health.max)
								unit->health.current = unit->health.max;

							level.selectedUnit->active = false;
							level.state = STATE_IDLE;
							if (level.unitEndsTurn != NULL)
								level.unitEndsTurn(level, scene, level.selectedUnit, level.selectedUnit->x / TILE_SIZE, level.selectedUnit->y / TILE_SIZE);
							return true;
						}
					}
				}
			} // end if(colliding)
		} // end looping through units

		 //loop player's buildings
		for (u16 i = 0; i < current->buildings.size(); ++i) {
			Building* building = &current->buildings[i];
			BuildingType type = building->type;

			//if building is active and a unit production one
			if (building->active && is_production_building(type) && hoveredUnit == NULL) {
				//if building is clicked
				bool buildingColliding = colliding(rect(building->x + level.xPos, building->y + level.yPos, TILE_SIZE, TILE_SIZE), mouse_pos.x, mouse_pos.y);

				if (buildingColliding && is_button_released(MOUSE_BUTTON_LEFT)) {
					//play_sound(button_press_sound);

					//if player isn't an AI and it is the player's turn
					if (!current->AI && currPlayer == (level.currentTurn & OWNER_COUNT)) {
						if (level.state == STATE_IDLE) {
							level.state = STATE_TRAINING_MENU;
							level.selectedBuilding = building;
							return true;
						}
					} // end if(player isn't AI and it's the player's turn)
				} // end if(is building moused and mouse button released)
			} // end if(building is active and a unit production building)
		} // looping through buildings

	} // end looping through players

	if (level.state == STATE_IDLE && hoveredUnit != NULL) {
		const u8 width = 5;
		const u8 height = 4;
		const u16 panelWidth = scene->menu[0].width * (width + 1);
		const u16 panelHeight = scene->menu[0].height * (height + 1);
		const i32 x = get_virtual_width() - panelWidth;
		const i32 y = get_virtual_height() - panelHeight;
		draw_panel(scene->menu, x, y, width, height);
		u16 currY = y - 11;
		const i32 text_X = x + 40;

		draw_texture_EX(scene->unitsheet, hoveredUnit->src, rect(x, y + (panelHeight / 2) - (TILE_SIZE / 2), TILE_SIZE, TILE_SIZE));
		draw_text(scene->body, format_text("Damage: %d", hoveredUnit->damage), text_X, currY += 15, BLACK.x, BLACK.y, BLACK.z);
		draw_text(scene->body, format_text("Health: %d/%d", (i32)hoveredUnit->health.shown, (i32)hoveredUnit->health.max), text_X, currY += 15, BLACK.x, BLACK.y, BLACK.z);
		draw_text(scene->body, format_text("Speed:  %d", hoveredUnit->moveRange), text_X, currY += 15, BLACK.x, BLACK.y, BLACK.z);
	}

	if (level.state == STATE_IDLE && is_button_released(MOUSE_BUTTON_RIGHT) && hoveredUnit == NULL) {
		level.menu.xPos = get_virtual_width() - (scene->menu[0].width * 3);
		level.menu.yPos = 30;
		level.state = STATE_MENU;
		return true;
	}

	if (level.state == STATE_AI) {
		take_ai_turn(level, level.players[level.currentTurn], scene);
	}

	if (level.state == STATE_CUTSCENE) {
		if (level.actions.size() == 0)
			level.state = STATE_IDLE;
		if (level.actions.size() > 0 && level.actions.at(0)->perform(&level, scene)) {
			delete level.actions.at(0);
			level.actions.erase(level.actions.begin());
		}
	}

	//if moving, move unit to tile if a valid tile is clicked.
	if (level.state == STATE_MOVESTATE) {
		for (u16 i = 0; i < level.move_tiles.size(); ++i) {
			vec2 tile = level.move_tiles[i];
			if (colliding(rect((tile.x * TILE_SIZE) + level.xPos, (tile.y * TILE_SIZE) + level.yPos, TILE_SIZE, TILE_SIZE), mouse_pos.x, mouse_pos.y)) {
				if (level.path.size() > 0)
					level.path.pop_back();
				level.path.push_back(tile);

				if (is_button_released(MOUSE_BUTTON_LEFT) && level.selectedUnit != NULL) {
					//play_sound(button_press_sound);

					level.prev_unit_x = level.selectedUnit->x;
					level.prev_unit_y = level.selectedUnit->y;

					level.selectedUnit->path = pathfind(
						&level, level.selectedUnit,
						move_heuristic,
						V2(level.selectedUnit->x / TILE_SIZE, level.selectedUnit->y / TILE_SIZE),
						tile
					);
					level.state = STATE_MOVING;
					return true;
				}
			}
		}
	}

	if (level.state == STATE_MOVING) {
		if (move_unit(&level, level.selectedUnit, false)) {
			level.state = STATE_UNIT_MENU;
			return true;
		}
	}

	return false;
}

// LEVEL
void draw_level(Level& level, LevelScene* scene, vec2 mouse_pos) {
	//draw solid color background
	draw_rectangle(0, 0, get_virtual_width(), get_virtual_height(), SKYBLUE);
	//draw map
	draw_map(level.map, level.xPos, level.yPos);
	//draw tiles
	if (level.state == STATE_MOVESTATE) {
		draw_tiles(level, level.move_tiles, 30, 190, 30, 120);
		draw_tiles(level, level.path, 30, 30, 190, 120);
	}
	if (level.state == STATE_ATTACKING) {
		draw_tiles(level, level.attack_tiles, 190, 30, 30, 120);
	}
	if (level.state == STATE_HEALING) {
		draw_tiles(level, level.attack_tiles, 30, 30, 150, 120);
	}

	Rect dst;
	dst.width = dst.height = TILE_SIZE;
	//loop players
	for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
		Player* current = &level.players[currPlayer];

		//loop buildings
		for (u16 i = 0; i < current->buildings.size(); ++i) {
			Building* building = &current->buildings[i];
			//draw buildings
			dst.x = building->x + level.xPos;
			dst.y = building->y + level.yPos;
			draw_texture_EX(scene->buildingsheets[building->owner], building->src, dst);
		}
	}

	//loop players
	for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
		Player* current = &level.players[currPlayer];

		//loop units
		for (u16 i = 0; i < current->units.size(); ++i) {
			Unit* unit = &current->units[i];
			//draw units
			dst.x = unit->x + level.xPos;
			dst.y = unit->y + level.yPos;
			if (unit->active || current->AI)
				draw_texture_EX(scene->unitsheet, unit->src, dst);
			else
				draw_texture_EX(scene->unitsheet, unit->src, dst, GREY_TINT);

			//update and draw health
			update_health(unit->health, scene->healthBar, current, unit->x + level.xPos, unit->y + level.yPos);

			//check if unit died
			if (unit->health.shown <= 0) {
				current->units.erase(current->units.begin() + i);
			}
		}
	}

	//draw attack animation
	//draw_animation(level.attackAnim, level.attackAnimPos.x + level.xPos, level.attackAnimPos.y + level.yPos);

	Player* currPlayer = &level.players[level.currentTurn];
	//draw UI showing allies of current player
	//draw UI showing money of current player
	draw_panel(scene->menu, get_virtual_width() - 80, 0, 3, 1);
	draw_text(scene->body, format_text("Funds: $%d", currPlayer->money), get_virtual_width() - 75, 5, BLACK.x, BLACK.y, BLACK.z);
	//draw UI showing day
	draw_panel(scene->menu, -18, 0, 2, 1);
	draw_text(scene->body, format_text("Day %d", level.currentDay), 5, 5, BLACK.x, BLACK.y, BLACK.z);

	if (level.state == STATE_SPLASH) {
		if (level.nextTurnSplash.draw()) {
			//next turn starts, all end-turn effects are activated
			Player* currPlayer = &level.players[level.currentTurn];
			level.currentTurn++;
			//skip turn of players with 0 units
			while (level.players[level.currentTurn].units.size() == 0) {
				level.currentTurn++;
			}
			if (level.currentTurn >= OWNER_COUNT) {
				level.currentTurn = 0;
				level.currentDay++;
			}

			//center level camera on the first unit of the next player
			Player* next = &level.players[level.currentTurn];

			if (next->units.size() > 0 && !next->AI) {
				level.xPos = -next->units[0].x + (get_virtual_width() / 2);
				level.yPos = -next->units[0].y + (get_virtual_height() / 2);
			}

			if (level.players[level.currentTurn].AI)
				level.state = STATE_AI;
			else
				level.state = STATE_IDLE;
		}
	}

	//draw menu and do things when buttons are pressed
	if (level.state == STATE_UNIT_MENU) {
		if (level.selectedUnit == NULL) {
			BMT_LOG(FATAL_ERROR, "Somehow the selected unit became unselected while in the menu.");
		}
		u16 rows = 2;
		if (level.selectedUnit->attributes & ATTR_HEALER)
			rows++;
		if (level.selectedUnit->attributes & ATTR_BUILDER)
			rows++;

		level.menu.yPos = level.selectedUnit->y - (scene->menu[0].height * (rows + 2)) + level.yPos;
		level.menu.xPos = level.selectedUnit->x - (TILE_SIZE / 2) + level.xPos;
		level.menu.row(rows, scene->unitMenu);
		level.menu.background(scene->unitMenu);
		//attack option
		if (level.menu.push_button("Attack", mouse_pos, scene->body)) {
			level.attack_tiles = find_range(&level, level.selectedUnit->attackRange, level.selectedUnit, attack_heuristic);
			level.state = STATE_ATTACKING;
			return;
		}
		//attribute-based options
		if (level.selectedUnit->attributes & ATTR_HEALER) {
			if (level.menu.push_button("Heal", mouse_pos, scene->body)) {
				level.attack_tiles = find_range(&level, level.selectedUnit->attackRange, level.selectedUnit, attack_heuristic);
				level.state = STATE_HEALING;
				return;
			}
		}
		if (level.selectedUnit->attributes & ATTR_BUILDER) {
			if (level.menu.push_button("Build", mouse_pos, scene->body)) {
				level.state = STATE_BUILDING_MENU;
				return;
			}
		}
		//wait option
		if (level.menu.push_button("Wait", mouse_pos, scene->body)) {
			Unit* unit = level.selectedUnit;
			unit->active = false;
			level.state = STATE_IDLE;
			if (level.unitEndsTurn != NULL)
				level.unitEndsTurn(level, scene, level.selectedUnit, unit->x / TILE_SIZE, unit->y / TILE_SIZE);
			return;
		}
	}

	if (level.state == STATE_MENU) {
		level.menu.row(4, scene->menu);
		level.menu.background(scene->menu);
		if (level.menu.push_button("Abilities", mouse_pos, scene->body)) {
			//state = STATE_COMMANDER_ABILITIES;
		}
		if (level.menu.push_button("Options", mouse_pos, scene->body)) {
			//state = STATE_OPTIONS;
		}
		if (level.menu.push_button("End Turn", mouse_pos, scene->body)) {
			level.nextTurnSplash.xPos = 0;
			level.nextTurnSplash.yPos = (get_virtual_height() / 2) - (scene->nextTurn.height / 2);
			level.nextTurnSplash.begin(scene->nextTurn, SPLASH_FADE_SPEED);
			level.state = STATE_SPLASH;
			activate_building_effects(level);

			if (level.turnEnds != NULL) {
				level.turnEnds(level, scene, level.currentTurn, level.currentDay);
				return;
			}
		}
		if (level.menu.push_button("Back", mouse_pos, scene->body)) {
			level.state = STATE_IDLE;
			return;
		}
	}

	if (level.state == STATE_BUILDING_MENU) {
		if (level.selectedUnit == NULL) {
			BMT_LOG(FATAL_ERROR, "Somehow the selected unit became unselected while in the build menu.");
		}
		Player* player = &level.players[level.currentTurn];
		Faction* faction = &player->faction;

		u32 rows = faction->buildings.size() + 2;
		level.menu.yPos = level.selectedUnit->y - (scene->menu[0].height * (rows + 2)) + level.yPos;
		level.menu.xPos = level.selectedUnit->x - ((scene->menu[0].width * 4) / 2) + level.xPos;
		level.menu.width = 6;
		level.menu.center = false;
		level.menu.row(rows, scene->menu);
		level.menu.background(scene->menu);
		for (u16 i = 0; i < faction->buildings.size(); ++i) {
			draw_text(scene->body, format_text("%d", faction->buildings[i].cost), level.menu.xPos + 110, level.menu.curr_y, BLACK.x, BLACK.y, BLACK.z);
			if (level.menu.push_button(format_text("%s", faction->buildings[i].name.c_str()), mouse_pos, scene->body)) {
				if (player->money >= faction->buildings[i].cost) {
					player->money -= faction->buildings[i].cost;
					spawn_building(level, faction->buildings[i].type, (Owner)level.currentTurn, level.selectedUnit->x / TILE_SIZE, level.selectedUnit->y / TILE_SIZE);
					level.selectedUnit->active = false;
					level.state = STATE_IDLE;
				}
				else {
					//play_sound(invalid_sound);
				}

				level.menu.width = 2;
				level.menu.center = true;
				return;
			}
		}
		if (level.menu.push_button("Back", mouse_pos, scene->body)) {
			level.menu.width = 2;
			level.menu.center = true;
			if (level.selectedUnit != NULL) {
				level.selectedUnit->x = level.prev_unit_x;
				level.selectedUnit->y = level.prev_unit_y;
			}
			level.state = STATE_MOVESTATE;
			return;
		}
		level.menu.width = 2;
		level.menu.center = true;
	}

	if (level.state == STATE_TRAINING_MENU) {
		if (level.selectedBuilding == NULL) {
			BMT_LOG(FATAL_ERROR, "Somehow the selected building became unselected while in the build menu.");
		}
		Player* player = &level.players[level.currentTurn];
		Faction* faction = &player->faction;
		BuildingTypeData building = get_type_data(level.selectedBuilding->type);

		u32 rows = building.trainable.size() + 1;
		level.menu.yPos = level.selectedBuilding->y - (scene->menu[0].height * (rows + 2)) + level.yPos;
		level.menu.xPos = level.selectedBuilding->x - ((scene->menu[0].width * 4) / 2) + level.xPos;
		level.menu.width = 6;
		level.menu.center = false;
		level.menu.row(rows, scene->menu);
		level.menu.background(scene->menu);
		for (u16 i = 0; i < building.trainable.size(); ++i) {
			draw_text(scene->body, format_text("%d", building.trainable[i].cost), level.menu.xPos + 110, level.menu.curr_y, BLACK.x, BLACK.y, BLACK.z);
			if (level.menu.push_button(format_text("%s", building.trainable[i].name.c_str()), mouse_pos, scene->body)) {
				if (player->money >= building.trainable[i].cost) {
					player->money -= building.trainable[i].cost;
					spawn_unit(level, building.trainable[i].type, (Owner)level.currentTurn, level.selectedBuilding->x / TILE_SIZE, level.selectedBuilding->y / TILE_SIZE);
					level.state = STATE_IDLE;
				}
				else {
					//play_sound(invalid_sound);
				}

				level.menu.width = 2;
				level.menu.center = true;
				return;
			}
		}
		if (level.menu.push_button("Back", mouse_pos, scene->body)) {
			level.state = STATE_IDLE;
			level.menu.width = 2;
			level.menu.center = true;
			return;
		}
		level.menu.width = 2;
		level.menu.center = true;
	}

	//if right mouse button clicked, reset back to previous state.
	if (is_button_released(MOUSE_BUTTON_RIGHT)) {
		//play_sound(revert_sound);
		if (level.state == STATE_MOVESTATE) {
			level.state = STATE_IDLE;
			level.selectedUnit = NULL;
			level.move_tiles.clear();
			return;
		}
		if (level.state == STATE_UNIT_MENU) {
			if (level.selectedUnit != NULL) {
				level.selectedUnit->x = level.prev_unit_x;
				level.selectedUnit->y = level.prev_unit_y;
			}
			level.state = STATE_MOVESTATE;
			return;
		}
		if (level.state == STATE_ATTACKING || level.state == STATE_HEALING) {
			level.attack_tiles.clear();
			level.state = STATE_UNIT_MENU;
			return;
		}
		if (level.state == STATE_BUILDING_MENU) {
			if (level.selectedUnit != NULL) {
				level.selectedUnit->x = level.prev_unit_x;
				level.selectedUnit->y = level.prev_unit_y;
			}
			level.state = STATE_MOVESTATE;
			return;
		}
		if (level.state == STATE_TRAINING_MENU) {
			level.state = STATE_IDLE;
			return;
		}
		if (level.state == STATE_MENU) {
			level.state = STATE_IDLE;
			return;
		}
	}

	//update the level at the end.
	if (level.sleepUpdateTime > 0) {
		level.sleepUpdateTime--;
	}
	else if (level.sleepUpdateTime < 0) {
		level.sleepUpdateTime = 0;
	}
	else {
		if (update_level(level, mouse_pos, scene))
			return;
	}
	return;
}

void spawn_unit(Level& level, UnitType type, Owner owner, u32 xPos, u32 yPos) {
	Unit unit = create_unit(type, owner, xPos, yPos);
	//TODO: If a unit is already on the xPos and yPos, pick the closest possible spot next to them.
	unit.active = false;
	level.players[owner].units.push_back(unit);
	std::swap(level.players[owner].units[0], level.players[owner].units[level.players[owner].units.size() - 1]);
}

void spawn_building(Level& level, BuildingType type, Owner owner, u32 xPos, u32 yPos) {
	Building unit = create_building(type, owner, xPos, yPos);
	//TODO: If a unit is already on the xPos and yPos, pick the closest possible spot next to them.
	unit.active = false;
	level.players[owner].buildings.push_back(unit);
	std::swap(level.players[owner].buildings[0], level.players[owner].buildings[level.players[owner].buildings.size() - 1]);
}

void dispose_level(Level& level) {
	dispose_map(level.map);
}

void draw_editor(Level& level, LevelScene* scene, vec2 mouse_pos) {

}

#ifdef _DEBUG_MODE
#include <sstream>
#include <iterator>
#include <string>

typedef std::vector<std::string> StringList;

void draw_level_debug_panel(Level& level, LevelScene* scene) {
	STORAGE bool show = false;
	STORAGE StringList log;
	STORAGE std::string input;
	const u32 xPos = get_virtual_width() - 200;

	if (is_key_pressed('`')) {
		show = !show;
		return;
	}

	if (show) {
		//draw debug info
		vec2 mouse_pos = get_mouse_pos();
		draw_rectangle(xPos, 0, 200, 270, V4(25, 25, 25, 195));
		u16 currY = -5;

		Unit* hoveredUnit = NULL;
		for (u8 currPlayer = 0; currPlayer < OWNER_COUNT; ++currPlayer) {
			Player* current = &level.players[currPlayer];
			for (u16 i = 0; i < current->units.size(); ++i) {
				Unit* unit = &current->units[i];
				bool unitColliding = colliding(rect(unit->x + level.xPos, unit->y + level.yPos, TILE_SIZE, TILE_SIZE), mouse_pos.x, mouse_pos.y);
				if (unitColliding) hoveredUnit = unit;
			}
		}

		draw_text(scene->body, format_text("Tile Moused Over: (%d, %d)", (i32)((mouse_pos.x - level.xPos) / TILE_SIZE), (i32)((mouse_pos.y - level.yPos) / TILE_SIZE)), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("Current turn: %d", level.currentTurn), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("Current day: %d", level.currentDay), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("level.xPos: %d", level.xPos), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("level.yPos: %d", level.yPos), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("sleepUpdateTime: %d", level.sleepUpdateTime), xPos + 5, currY += 15);
		Player* currPlayer = &level.players[level.currentTurn];
		draw_text(scene->body, format_text("currentPlayer: %d", level.currentTurn), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.AI: %s", currPlayer->AI ? "true" : "false"), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.money: %d", currPlayer->money), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.numUnits: %d", currPlayer->units.size()), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.numBuildings: %d", currPlayer->buildings.size()), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.faction: %s", currPlayer->faction.name.c_str()), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("currentPlayer.lastSelectedUnit: %d", currPlayer->lastSelectedUnit), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("level.state: %d", level.state), xPos + 5, currY += 15);
		draw_text(scene->body, format_text("level.path.size(): %d", level.path.size()), xPos + 5, currY += 15);
		if (hoveredUnit != NULL)
			draw_text(scene->body, format_text("Hovered Unit type: %d", hoveredUnit->type), xPos + 5, currY += 15);

		//draw console
		draw_rectangle(0, 0, 240, 335, V4(25, 25, 25, 195));

		text_input_field_fixed(&input, 0, 336, 239, 20, WHITE, V4(0, 0, 0, 195), WHITE.xyz, scene->body);
		for (u16 i = 0; i < log.size(); ++i) {
			draw_text(scene->body, log[i], 5, i * get_font_height(scene->body));
		}
		if (log.size() > 24) {
			log.erase(log.begin());
		}

		if (is_key_pressed(KEY_ENTER)) {
			std::istringstream iss(input);
			std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
				std::istream_iterator<std::string>());
			if (results.size() == 0) return;
			log.push_back(input);

			if (results[0] == "help" || results[0] == "?") {
				log.push_back("spawn [type] [owner] [x] [y]");
				log.push_back("change_unit_owner [ID] [new_owner]");
				log.push_back("change_building_owner [ID] [new_owner]");
				log.push_back("clear");
				log.push_back("hide");
			}
			if (results[0] == "spawn") {
				if (results.size() == 5)
					spawn_unit(level, (UnitType)atoi(results[1].c_str()), (Owner)atoi(results[2].c_str()), atoi(results[3].c_str()), atoi(results[4].c_str()));
				else
					log.push_back("usage: spawn [type] [owner] [x] [y]");
			}
			if (results[0] == "change_owner") {

			}
			if (results[0] == "clear") {
				if (results.size() > 1) {
					log.push_back("usage: clear");
				}
				else {
					log.clear();
				}
			}
			if (results[0] == "hide") {
				if (results.size() > 1) {
					log.push_back("usage: hide");
				}
				else {
					show = false;
				}
			}

			input.clear();
		}
	}
}
#endif
