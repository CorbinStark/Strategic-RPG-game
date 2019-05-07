#ifndef LEVEL_H
#define LEVEL_H

#include <bahamut.h>
#include "globals.h"
#include "game_objects.h"
#include "gui.h"
#include <SOIL.h>

#define MOVE_SPEED 4
#define MAX_PATHFIND_TRIES 500
#define SPLASH_FADE_SPEED 2.4

internal const vec4 GREY_TINT = V4(120, 120, 120, 255);

struct Map {
	i32* grid;
	i32 width;
	i32 height;
	Texture tilesheet1;
	Texture tilesheet2;
};

struct Node;
typedef std::shared_ptr<Node> NodePtr;
struct Node {
	Node();
	Node(int x, int y, NodePtr parent, double gCost, double hCost);
	int x;
	int y;
	NodePtr parent;
	int gCost;
	int hCost;
	int fCost;
};

enum LevelState {
	STATE_IDLE,
	STATE_MOVESTATE,
	STATE_MOVING,
	STATE_ATTACKING,
	STATE_HEALING,
	STATE_MENU,
	STATE_UNIT_MENU,
	STATE_BUILDING_MENU,
	STATE_TRAINING_MENU,
	STATE_SPLASH,
	STATE_ANIMATION,
	STATE_CUTSCENE,
	STATE_AI,
	STATE_NOTIFICATION,
	LEVELSTATE_COUNT
};

struct Action;

struct LevelScene {
	Texture unitsheet;
	Texture buildingsheets[5];
	Texture healthBar;
	Texture unitMenu[9];
	Texture menu[9];
	Texture nextTurn;

	Texture eleanor_normal;
	Texture eleanor_happy;
	Texture eleanor_angry;

	Font body;
};

struct Level {
	i16 yPos;
	i16 xPos;
	Menu menu;
	Splash nextTurnSplash;
	Player players[OWNER_COUNT];
	Map map;
	LevelState state = STATE_IDLE;
	Unit* selectedUnit;
	Building* selectedBuilding;
	i32 prev_unit_x;
	i32 prev_unit_y;
	i32 currentTurn;
	i32 currentDay;
	std::vector<vec2> move_tiles;
	std::vector<vec2> attack_tiles;
	std::vector<vec2> path;
	std::vector<Action*> actions;
	u16 sleepUpdateTime;

	void(*unitDies)(Level& level, LevelScene* scene, Unit* died, i32 tileX, i32 tileY) = NULL;
	void(*unitIsAttacked)(Level& level, LevelScene* scene, Unit* died, i32 tileX, i32 tileY) = NULL;
	void(*unitEndsTurn)(Level& level, LevelScene* scene, Unit* unit, i32 tileX, i32 tileY) = NULL;
	void(*turnEnds)(Level& level, LevelScene* scene, i32 turn, i32 day) = NULL;
};

internal inline
void load_scene(LevelScene* scene, Config* config) {
	scene->buildingsheets[0] = load_texture("data/art/buildings/buildingsheet_green.png", config->texParam);
	scene->buildingsheets[1] = load_texture("data/art/buildings/buildingsheet_red.png", config->texParam);
	scene->buildingsheets[2] = load_texture("data/art/buildings/buildingsheet_blue.png", config->texParam);
	scene->buildingsheets[3] = load_texture("data/art/buildings/buildingsheet_yellow.png", config->texParam);
	scene->buildingsheets[4] = load_texture("data/art/buildings/buildingsheet_neutral.png", config->texParam);

	scene->eleanor_happy = load_texture("data/art/characters/eleanor_happy.png", config->texParam);
	scene->eleanor_angry = load_texture("data/art/characters/eleanor_angry2.png", config->texParam);
	scene->eleanor_normal = load_texture("data/art/characters/eleanor_normal.png", config->texParam);

	scene->unitsheet = load_texture("data/art/unitsheet.png", config->texParam);
	scene->healthBar = load_texture("data/art/health_bar.png", config->texParam);

	scene->nextTurn = load_texture("data/art/next_turn_splash.png", config->texParam);
	scene->body = config->body;

	i32 w;
	i32 h;
	u16 i = 0;
	unsigned char* image = SOIL_load_image("data/art/unitmenu.png", &w, &h, 0, SOIL_LOAD_RGBA);
	for (u8 x = 0; x < 3; ++x)
		for (u8 y = 0; y < 3; ++y)
			scene->unitMenu[i++] = get_sub_image(image, w, x * (w / 3), y * (h / 3), w / 3, h / 3, config->texParam);
	free(image);

	i = 0;
	image = SOIL_load_image("data/art/menu.png", &w, &h, 0, SOIL_LOAD_RGBA);
	for (u8 x = 0; x < 3; ++x)
		for (u8 y = 0; y < 3; ++y)
			scene->menu[i++] = get_sub_image(image, w, x * (w / 3), y * (h / 3), w / 3, h / 3, config->texParam);
	free(image);
}

internal inline
void dispose_scene(LevelScene* scene) {
	for (u16 i = 0; i < 5; ++i)
		dispose_texture(scene->buildingsheets[i]);

	dispose_texture(scene->eleanor_angry);
	dispose_texture(scene->eleanor_happy);
	dispose_texture(scene->eleanor_normal);

	dispose_texture(scene->unitsheet);
	dispose_texture(scene->healthBar);
	dispose_texture(scene->nextTurn);

	for (u16 i = 0; i < 9; ++i)
		dispose_texture(scene->unitMenu[i]);

	for (u16 i = 0; i < 9; ++i)
		dispose_texture(scene->menu[i]);
}

void draw_editor(Level& level, LevelScene* scene, vec2 mouse_pos);

void spawn_unit(Level& level, UnitType type, Owner owner, u32 xPos, u32 yPos);
void spawn_building(Level& level, BuildingType type, Owner owner, u32 xPos, u32 yPos);
Level load_level(const char* path, Texture tilesheetOne, Texture tilesheetTwo);
void save_level(const char* path);
void draw_level(Level& level, LevelScene* scene, vec2 mouse_pos);
void draw_level_debug_panel(Level& level, LevelScene* scene);
void dispose_level(Level& level);

struct Action {
	virtual bool perform(Level* level, LevelScene* scene) = 0;
};

struct PanAction : public Action {
	PanAction(vec2 dest) {
		this->dest = dest;
	}
	bool perform(Level* level, LevelScene* scene) {
		if (level->xPos <= dest.x) {
			level->xPos += MOVE_SPEED;
		}
		if (level->xPos >= dest.x) {
			level->xPos -= MOVE_SPEED;
		}
		if (level->yPos <= dest.y) {
			level->yPos += MOVE_SPEED;
		}
		if (level->yPos >= dest.y) {
			level->yPos -= MOVE_SPEED;
		}
		if (level->xPos > (dest.x - MOVE_SPEED) && level->xPos < (dest.x + MOVE_SPEED)) {
			if (level->yPos > (dest.y - MOVE_SPEED) && level->yPos < (dest.y + MOVE_SPEED)) {
				return true;
			}
		}
		return false;
	}
	vec2 dest;
};

struct DialogueAction : public Action {
	DialogueAction(std::string name, std::string text, Texture* tex) {
		this->name = name;
		this->tex = tex;
		
		std::string line;
		for (u32 i = 0; i < text.size(); ++i) {
			if (text[i] != '\n' && i != text.size() - 1)
				line.push_back(text[i]);
			else {
				lines.push_back(line);
				line.clear();
			}
		}
	}
	bool perform(Level* level, LevelScene* scene) {
		draw_panel(scene->menu, 50, get_virtual_height() - 100, 25, 25);
		draw_texture(*tex, 450, 200);

		for(u32 i = 0; i < lines.size(); ++i)
			draw_text(scene->body, lines[i], 60, (get_virtual_height() - 90) + (i * 15), BLACK.x, BLACK.y, BLACK.z);

		i32 key_released = get_key_released();
		i32 button_released = get_button_released();
		if (key_released > 0 || button_released > 0 || is_button_released(MOUSE_BUTTON_LEFT))
			return true;
		return false;
	}
	std::string name;
	StringList lines;
	Texture* tex;
};

struct SpawnAction : public Action {
	SpawnAction(UnitType type, Owner owner, i32 x, i32 y, u32 delay) {
		this->type = type;
		this->owner = owner;
		this->x = x;
		this->y = y;
		this->delay = delay;
	}
	bool perform(Level* level, LevelScene* scene) {
		delay--;
		if (delay > 0) return false;
		spawn_unit(*level, type, owner, x, y);
		return true;
	}
	UnitType type;
	Owner owner;
	i32 x;
	i32 y;
	u32 delay;
};

struct SetTurnAction : public Action {
	SetTurnAction(u8 turn) {
		this->turn = turn;
	}
	bool perform(Level* level, LevelScene* scene) {
		level->nextTurnSplash.xPos = 0;
		level->nextTurnSplash.yPos = (get_virtual_height() / 2) - (scene->nextTurn.height / 2);
		level->nextTurnSplash.begin(scene->nextTurn, SPLASH_FADE_SPEED);
		level->state = STATE_SPLASH;

		Player* currPlayer = &level->players[level->currentTurn];
		for (u16 currUnit = 0; currUnit < currPlayer->units.size(); ++currUnit) {
			currPlayer->units[currUnit].active = true;
		}

		level->currentTurn = turn;

		if (level->players[level->currentTurn].AI)
			level->state = STATE_AI;
		else
			level->state = STATE_IDLE;

		return true;
	}
	u8 turn;
};

struct AddFundsAction : public Action {
	AddFundsAction(u8 player, i32 funds) {
		this->player = player;
		this->funds = funds;
	}
	bool perform(Level* level, LevelScene* scene) {
		level->players[player].money += funds;
		return true;
	}
	u8 player;
	i32 funds;
};

struct ExpositionActionSlice;
struct ExpositionAction : public Action {
	ExpositionAction(bool fade, f32 speed, f32 delay, Font& font, std::string text, f32 startAlpha = 0) {
		this->fade = fade;
		this->speed = speed;
		this->delay = delay;
		this->font = font;
		alpha = startAlpha;
		currentLetter = 0;
		currentLine = 0;

		text.append(" ");
		std::string line;
		for (u32 i = 0; i < text.size(); ++i) {
			if (text[i] != '\n' && i != text.size() - 1)
				line.push_back(text[i]);
			else {
				lines.push_back(line);
				line.clear();
			}
		}
	}
	bool perform(Level* level, LevelScene* scene) {
		draw_rectangle(0, 0, get_virtual_width(), get_virtual_height(), 0, 0, 0, alpha);
		for (u32 i = 0; i < lines.size(); ++i) {
			u32 xPos = (get_virtual_width() / 2) - (get_string_width(font, lines.at(i).c_str()) / 2);

			if (i == currentLine) {
				for (u32 j = 0; j < currentLetter; ++j) {
					i32 yOffset = (font.characters['T']->bearing.y - font.characters[lines.at(i)[j]]->bearing.y) + 1;
					if (yOffset < 0) yOffset = 0;

					draw_texture(font.characters[lines.at(i)[j]]->texture, xPos, (i * 20) + yOffset, 255, 255, 255, alpha);
					xPos += (font.characters[lines.at(i)[j]]->advance >> 6);
				}
			}
			else if (i < currentLine) {
				for (u32 j = 0; j < lines.at(i).size(); ++j) {
					i32 yOffset = (font.characters['T']->bearing.y - font.characters[lines.at(i)[j]]->bearing.y) + 1;
					if (yOffset < 0) yOffset = 0;

					draw_texture(font.characters[lines.at(i)[j]]->texture, xPos, (i * 20) + yOffset, 255, 255, 255, alpha);
					xPos += (font.characters[lines.at(i)[j]]->advance >> 6);
				}
			}
		}
		if (alpha < 255 && currentLine != lines.size()) alpha += speed;
		if (delay > 0) delay--;
		if (delay <= 0 && currentLine != lines.size()) {
			delay = 4;
			if (lines.at(currentLine).size() > 0 && lines.at(currentLine).at(currentLetter) == '.')
				delay = 15;
			currentLetter++;
			if (currentLetter >= lines.at(currentLine).size()) {
				currentLetter = 0;
				currentLine++;
			}
		}
		if (currentLine > 19) {
			currentLine = 0;
			delay = 0;
			for (u32 i = 0; i < 19-1; ++i) {
				lines.erase(lines.begin());
			}
		}
		if (alpha > 0 && currentLine == lines.size()) {
			alpha -= speed;
		}
		if (alpha <= 0 && currentLine == lines.size()) {
			return true;
		}
		return false;
	}
	ExpositionAction* add_slice(ExpositionActionSlice* slice) {
		slices.push_back(slice);
		return this;
	}
	bool fade;
	f32 speed;
	f32 delay;
	f32 alpha;
	i32 currentLetter;
	i32 currentLine;
	StringList lines;
	Font font;

	std::vector<ExpositionActionSlice*> slices;
};

struct ExpositionActionSlice {
	virtual bool perform(ExpositionAction* expo) = 0;
};

void push_cutscene_action(Level* level, Action* action);
void begin_cutscene(Level* level);

#endif
