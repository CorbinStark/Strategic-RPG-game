#ifndef GUI2_H
#define GUI2_H

#include <defines.h>
#include <maths.h>
#include "globals.h"

enum Allignment {
	ALLIGN_LEFT,
	ALLIGN_RIGHT,
	ALLIGN_CENTER,
	ALLIGN_JUSTIFY,
	ALLIGN_NONE
};

enum InputRestrictions {
	INPUT_CHARS_ONLY,
	INPUT_NUMBERS_ONLY,
	INPUT_ALL_ALLOWED
};

bool draw_text_button(const char* const text, const i16 xPos, const i16 yPos, vec2 mousePos);
void text_input_field(
	std::string* output,
	const i16 xPos,
	const i16 yPos,
	const u16 width,
	const u16 height,
	vec4 fgcolor,
	vec4 bgcolor,
	vec3 textcolor,
	vec2 mousePos,
	Font font,
	InputRestrictions restriction = InputRestrictions::INPUT_ALL_ALLOWED
);

void text_input_field_fixed(
	std::string* output,
	const i16 xPos,
	const i16 yPos,
	const u16 width,
	const u16 height,
	vec4 fgcolor,
	vec4 bgcolor,
	vec3 textcolor,
	Font font,
	InputRestrictions restriction = InputRestrictions::INPUT_ALL_ALLOWED
);

struct Menu {
	i16 width;
	i16 height;
	i16 xPos;
	i16 yPos;
	i16 curr_x;
	i16 curr_y;
	i16 padding;
	bool center = true;
	Allignment allignmentX = Allignment::ALLIGN_NONE;
	Allignment allignmentY = Allignment::ALLIGN_NONE;
	Texture* bg;

	void row(u16 num_columns, Texture bg[9]);
	void background(Texture ninePatchRect[9]);
	void title(const char* title, Font font);
	bool push_button(const char* text, vec2 mousePos, Font font);
	void push_label(const char* text, Font font);
	void push_divider();
};

void draw_panel(Texture ninePatchRect[9], i16 xPos, i16 yPos, u16 width, u16 height);

//TODO: make this opaque
struct Splash {
	u16 xPos;
	u16 yPos;
	double fade_speed;
	double alpha;
	Texture tex;
	bool done;

	void begin(Texture tex, double fade_time);
	bool draw();
	void end();
};

struct Animation {
	Texture animationSheet;
	u8 numFrames;
	u8 frameWidth;
	u8 frameHeight;
	u8 row;
	u8 currentFrame;
	u16 delay;
	u16 timer;
	u64 flipFlag;
	bool playing;
	bool looping;
};

Animation create_animation(Texture sheet, u8 numFrames, u8 frameWidth, u8 frameHeight, u8 row, u16 delay);
void start_animation(Animation& animation, bool looping, u64 flipFlag = 0);
void stop_animation(Animation& animation);
void draw_animation(Animation& animation, i32 xPos, i32 yPos);

#endif