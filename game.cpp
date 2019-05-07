#include "globals.h"
#include "level.h"
#include "menu.h"
#include "states.h"
#include <bahamut.h>
#include <audio.h>
#include <iostream>

#ifndef _DEBUG_MODE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

/*
KEY:
--		not started
X-		started but not finished
XX		finished
??		possible feature
::		bug
////////////////////////////////////////////////////////////////////////
//                              TODO
////////////////////////////////////////////////////////////////////////
XX 1)	Create cutscene (or trigger) system to have more dynamic and
			interesting levels.
XX 2)	Create a generic melee animation (like a swiping effect)
?? 3)	Create intro cutscene
XX 4)	FIX: Pathfind() loops infinitely if it cannot find a path (e.g.
			if a unit is surrounded and another unit tries to path to it)
XX 5)	FIX: Units from other factions can move when it's not their turn.
XX 6)	Implement buildings (unit-trainers, money gatherers, healing stations)
-- 7)	Create menu interface for a hub area where you unlock new units,
			select an area to travel to, do missions, etc.
XX 8)	Splash screen for next turn
?? 9)	Add stamina system (aka a unit has to rest at a friendly building if
			they moved too much, their movespeed and damage is reduced while tired)
			visible animation should play to show this to player.
-- 10)	Add more unit types
XX 11)	Display unit stats when mousing over them in IDLE mode.
?? 12)	Global hero abilities
?? 13)	When units do not attack in a turn, they are automatically
			put in defence mode where they take less damage.
XX 14)	Make the state system in level a bit easier to read
XX 15)	Make players an actual struct instead of just an int denoting
			which team a unit is on.
-- 16)	Add death animation(s?)
:: 17)	FIX: AI behaves a bit wonky. Sometimes they'll walk up to an
			enemy and not attack them, and sometimes they'll walk away
			from an enemy mid-fight.
XX 18)	Remove '% OWNER_COUNT's from level.cpp and instead just have the currentTurn loop back to 0.
-- 19)	Local multiplayer functionality
XX 20)	FIX: Softlocks when there is only one enemy left.
XX 21)	FIX: Sometimes when reverting an action, it reverts it for the wrong unit.
-- 22)	When mousing over units in building menu, show unit stats to
			the right of the menu.
XX 23)	Add allies functionality
XX 24)	FIX: When AI attacks in range of multiple enemies, if the enemies
			are on different teams, it will attack one from every team.
XX 25)	Create unique factions with their own unit types and buildings
XX 26)	Loading and saving should be updated to work with buildings and players
XX 27)	Update menu art
X- 28)	Make building and unit art change color based on owner.
XX 29)	Add a general menu for ending turn, surrendering, options, etc.
-- 30)	AI should retreat to a friendly building if health drops below ~35%
-- 31)	Add ability to destroy enemy buildings
XX 32)	Add ability to train units
XX 33)	Make the AI also train units (and figure out how they will determine
			which unit to train next)
XX 34)	FIX: When a unit is on a building, and the player tries to select it,
				the player will select the building instead. It should select unit.
-- 35)	Add Passive and active abilities to units.
XX 36)	FIX: Pathfinding freaks out sometimes.
XX 37)	FIX: Sound doesn't work (alGenSources generates no sources AND
				generates no errors.)
-- 38)	Design skirmish (local multiplayer) maps and create a menu to select which
			one to skirmish on.
-- 39)	Fog of War on certain maps (determined on map by map basis)
X- 40)	Add AI personalities and change behaviour based on them
-- 41)	AI should use active abilities of units
////////////////////////////////////////////////////////////////////////
*/

#define EXIT_STATE_SUCCESS 0
#define EXIT_STATE_FAILURE 1

#include "menu.h"
#include "leveltriggers.h"

internal inline
void display(Config* config) {
	u16 currLevel = 1;
	LevelScene scene = { 0 };
	Shader basic = load_default_shader_2D();
	std::vector<Level> levels = load_levels(&scene, config);
	load_scene(&scene, config);

	begin2D(basic);
	upload_mat4(basic, "projection", orthographic_projection(0, 0, get_virtual_width(), get_virtual_height(), -1, 1));
	end2D();

	Texture cursor = load_texture("data/art/cursor.png", config->texParam);
	Texture demon = load_texture("data/art/characters/demon3.png", config->texParam);

	while (window_open()) {
		Rect viewport = fit_aspect_ratio((f32)get_virtual_width() / (f32)get_virtual_height());
		vec2 mousePos = get_mouse_pos_adjusted(viewport);
		set_viewport(viewport.x, viewport.y, viewport.width, viewport.height);
		begin_drawing();

		begin2D(basic);
			draw_level(levels[currLevel], &scene, mousePos);
#ifdef _DEBUG_MODE
			draw_level_debug_panel(levels[currLevel], &scene);
#endif
			draw_texture(cursor, mousePos.x, mousePos.y);
		//	draw_texture(demon, 0, 0);
		//	draw_rectangle(0, 0, 1920, 100, BLACK);
		//	draw_rectangle(0, 900, 1920, 300, BLACK);
		//	std::string str = "Lorem ipsum placeholder exposition text. This is the first of the demon characters I've drawn";
		//	draw_text(config->body, 
		//		str,
		//		(1920 / 2)-(get_string_width(config->body, str.c_str()) / 2), 920
		//	);
		//	std::string str2 = "This is a second line!";
		//	draw_text(config->body,
		//		str2,
		//		(1920 / 2) - (get_string_width(config->body, str2.c_str()) / 2), 960
		//	);
		end2D();

		end_drawing();
	}

	dispose_texture(cursor);
	dispose_window();
}

int main() {
	Config config = load_config("data/config.txt");
	init_context("Alek Bones and the Data Battle", &config);
	set_mouse_hidden(true);
	//set_window_size(640, 480);
	//set_window_pos(0, 30);
	preload_all_unit_and_building_types();

	display(&config);

	return EXIT_STATE_SUCCESS;
}
