#ifndef LEVEL_TRIGGERS_H
#define LEVEL_TRIGGERS_H
#include "level.h"

//this is where I hardcode all the level triggers.

void level_1_endturn_callback(Level& level, LevelScene* scene, i32 turn, i32 day) {
	if (day == 1) {
		push_cutscene_action(&level, new PanAction(V2(50, 85)));
		push_cutscene_action(&level, new DialogueAction("ELEANOR", "So this is the \"Globalist Proxy Pylon\" (whatever that\nis supposed to mean) that Alek mentioned?", &scene->eleanor_normal));
		push_cutscene_action(&level, new DialogueAction("ELEANOR", "THIS IS A NORMAL TOWN!\nI KNEW HE WAS JUST FUCKING WITH ME!\nTHAT GODDAMNED CONSPIRACY THEORIST!", &scene->eleanor_angry));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_ALEKSANDER_BONES, OWNER_RED, 9, 1, 50));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_ALEKSANDER_BONES, OWNER_RED, 9, 3, 50));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_MINDSLAVE, OWNER_RED, 7, 3, 50));
		push_cutscene_action(&level, new PanAction(V2(295, -80)));
		push_cutscene_action(&level, new DialogueAction("ELEANOR", "Okay...\nMaybe Alek was right about this one.", &scene->eleanor_happy));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_RECRUIT, OWNER_GREEN, 1, 9, 50));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_RECRUIT, OWNER_GREEN, 0, 7, 50));
		push_cutscene_action(&level, new SpawnAction(UNIT_TYPE_RECRUIT, OWNER_GREEN, 3, 8, 50));
		push_cutscene_action(&level, new DialogueAction("???", "All right let's take out this pylon before they finish\nsetting up their mind control device. Everyone have\ntheir protective tinfoil hats on?", &scene->eleanor_angry));
		push_cutscene_action(&level, new SetTurnAction(1));
		begin_cutscene(&level);
	}
	if (day > 1 && level.players[1].units.size() == 0) {
		i32 x = level.players[0].units[0].x;
		i32 y = level.players[0].units[0].y;
		push_cutscene_action(&level, new PanAction(V2(-x + (get_virtual_width() / 2), -y + (get_virtual_height() / 2))));
		push_cutscene_action(&level, new DialogueAction("ELEANOR", "What the hell was that?", &scene->eleanor_normal));
		push_cutscene_action(&level, new DialogueAction("???", "Those were monsters employed by the Globalists\nto defend their mind control pylons.", &scene->eleanor_angry));
		push_cutscene_action(&level, new DialogueAction("The Developer", "This is the end of the demo mission.", &scene->eleanor_angry));
		begin_cutscene(&level);
	}
}

void level_1_init_callback(Level& level, LevelScene* scene) {
	std::string expotext = R"FOO(
Inside the station cafe it was warm and light. There were windows lining the walls.
Beside each of the windows, there were tables and stools. Outside the windows it was raining.
A man in a thick coat walks through the door, dripping.
"God damnit, betty, the globalists are at it again." he said.
"They should have never given Hillary the power to control the weather"

A few months prior, Lily had graduated college with a degree in journalism.
Lily applied to dozens of local newspapers, no luck. 
Then out of nowhere, I got an email from a network called "Cyberdata Communications".
We scheduled the interview an hour from my reply. They said they were in a hurry.
I got in my car and rushed over, and here we are.
Anyways, the man in the thick coat walked over to my table and sat in front of me.
It was Alex Jones.
I had seen him before, he was a well-known conspiracy theorist.
I guess Cyberdata Communications was a parent network to Alex's show Infowars.
I didn't have any other options. Money was money.
Alex stands up and gives me a hug.
"You ready to fight the good fight?"
I had no idea what he was talking about.
)FOO";
	//push_cutscene_action(&level, new ExpositionAction(true, 1.5f, 8.0f, scene->body, expotext, 255));
	push_cutscene_action(&level, new PanAction(V2(295, -80)));
	push_cutscene_action(&level, new DialogueAction("ELEANOR", "Alek told me to follow this road if I want to see the\n\"truth\" about the \"Globalists\"...", &scene->eleanor_normal));
	push_cutscene_action(&level, new DialogueAction("ELEANOR", "I guess I have nothing better to do... and I'm being\npaid for this as well...", &scene->eleanor_normal));
	begin_cutscene(&level);
}

void level_2_init_callback(Level& level, LevelScene* scene) {
	push_cutscene_action(&level, new PanAction(V2(130, -500)));
	push_cutscene_action(&level, new DialogueAction("ELEANOR", "We're being overwhelmed! There are too many of them!\nAlek is coming with reinforcements but it will take\nabout 15 days to arrive.", &scene->eleanor_normal));
	push_cutscene_action(&level, new AddFundsAction(0, 200));
	push_cutscene_action(&level, new AddFundsAction(1, 600));
	push_cutscene_action(&level, new AddFundsAction(2, 600));
	push_cutscene_action(&level, new AddFundsAction(3, 600));
	begin_cutscene(&level);
}

std::vector<Level> load_levels(LevelScene* scene, Config* config) {
	std::vector<Level> levels;

	Level level = { 0 };
	level = load_level("data/maps/map1.txt", load_texture("data/art/grass_tiles_1.png", config->texParam), load_texture("data/art/grass_tiles_2.png", config->texParam));
	level.xPos = 350;
	level.yPos = 50;
	level.turnEnds = level_1_endturn_callback;
	level_1_init_callback(level, scene);
	levels.push_back(level);

	//level.turnEnds = level_1_endturn_callback;
	level = load_level("data/maps/map2.txt", load_texture("data/art/grass_tiles_1.png", config->texParam), load_texture("data/art/grass_tiles_2.png", config->texParam));
	level.xPos = -550;
	level.yPos = 250;

	level.xPos = 130;
	level.yPos = -500;

	level_2_init_callback(level, scene);
	levels.push_back(level);

	return levels;
}

#endif
