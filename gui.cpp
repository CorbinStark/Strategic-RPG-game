#include "gui.h"
#include <font.h>
#include <bahamut.h>

INTERNAL const vec4 HIGHLIGHT_COLOR = V4(212, 184, 152, 255);
INTERNAL const vec4 TEXT_COLOR = V4(25, 25, 25, 255);

INTERNAL
bool draw_text_button(const char* const text, const u32 text_width, const i16 xPos, const i16 yPos, vec2 mousePos, Font font) {
	const u8 padding = 3;

	const u32 text_height = font.characters['T']->texture.height;
	const Rect button = rect(xPos - padding, yPos, text_width + padding, text_height + padding);
	const bool collided = colliding(button, mousePos.x, mousePos.y);
	const bool buttonReleased = is_button_released(MOUSE_BUTTON_LEFT);

	if (collided) {
		draw_rectangle(xPos - padding, yPos, text_width + padding, text_height + padding, HIGHLIGHT_COLOR);
	}

	draw_text(font, text, xPos, yPos, TEXT_COLOR.x, TEXT_COLOR.y, TEXT_COLOR.z);

	return collided & buttonReleased;
}

bool draw_text_button(const char* const text, const i16 xPos, const i16 yPos, vec2 mousePos, Font font) {
	const u32 text_width = get_string_width(font, text);
	return draw_text_button(text, text_width, xPos, yPos, mousePos, font);
}

void text_input_field(std::string* output, const i16 xPos, const i16 yPos, const u16 width, const u16 height, vec4 fgcolor, vec4 bgcolor, vec3 textcolor, vec2 mousePos, Font font, InputRestrictions restriction) {
	STORAGE u16 timer = 0;
	timer++;
	const Rect button = rect(xPos, yPos, width, height);
	const bool collided = colliding(button, mousePos.x, mousePos.y);

	if (collided) {
		const int key = get_key_pressed();
		if (restriction == INPUT_CHARS_ONLY)
			if (key >= 'a' && key <= 'z')
				output->push_back(key);

		if (restriction == INPUT_NUMBERS_ONLY)
			if (key >= '0' && key <= '9')
				output->push_back(key);

		if (restriction == INPUT_ALL_ALLOWED)
			if ((key >= '!' && key <= '~') || key == KEY_SPACE)
				output->push_back(key);

		if (key == KEY_BACKSPACE && output->size() > 0)
			output->pop_back();
	}

	draw_rectangle(xPos - 1, yPos - 1, width + 2, height + 2, fgcolor);
	draw_rectangle(xPos, yPos, width, height, bgcolor);
	if (collided) {
		if (timer % 100 > 50)
			draw_text(font, format_text("%s_", output->c_str()), xPos + 3, yPos + (height / 2) - (font.characters['T']->texture.height / 2), textcolor.x, textcolor.y, textcolor.z);
		else
			draw_text(font, format_text("%s ", output->c_str()), xPos + 3, yPos + (height / 2) - (font.characters['T']->texture.height / 2), textcolor.x, textcolor.y, textcolor.z);
	}
	else
		draw_text(font, format_text("%s ", output->c_str()), xPos + 3, yPos + (height / 2) - (font.characters['T']->texture.height / 2), textcolor.x, textcolor.y, textcolor.z);
}

void text_input_field_fixed(std::string* output, const i16 xPos, const i16 yPos, const u16 width, const u16 height, vec4 fgcolor, vec4 bgcolor, vec3 textcolor, Font font, InputRestrictions restriction) {
	STORAGE u16 timer = 0;
	timer++;
	const Rect button = rect(xPos, yPos, width, height);

	const int key = get_key_pressed();
	if (restriction == INPUT_CHARS_ONLY)
		if (key >= 'a' && key <= 'z')
			output->push_back(key);

	if (restriction == INPUT_NUMBERS_ONLY)
		if (key >= '0' && key <= '9')
			output->push_back(key);

	if (restriction == INPUT_ALL_ALLOWED)
		if ((key >= '!' && key <= '~') || key == KEY_SPACE)
			output->push_back(key);

	if (key == KEY_BACKSPACE && output->size() > 0)
		output->pop_back();

	draw_rectangle(xPos - 1, yPos - 1, width + 2, height + 2, fgcolor);
	draw_rectangle(xPos, yPos, width, height, bgcolor);
	if (timer % 100 > 50)
		draw_text(font, format_text("%s_", output->c_str()), xPos + 3, yPos + (height / 2) - (font.characters['T']->texture.height / 2), textcolor.x, textcolor.y, textcolor.z);
	else
		draw_text(font, format_text("%s ", output->c_str()), xPos + 3, yPos + (height / 2) - (font.characters['T']->texture.height / 2), textcolor.x, textcolor.y, textcolor.z);
}

void Menu::background(Texture ninePatchRect[9]) {
	draw_panel(ninePatchRect, xPos, yPos, width, height);
}

void draw_panel(Texture ninePatchRect[9], i16 xPos, i16 yPos, u16 width, u16 height) {
	assert(width >= 1);
	assert(height >= 1);

	draw_texture(ninePatchRect[0], xPos, yPos);
	draw_texture(ninePatchRect[2], xPos, yPos + (height * ninePatchRect[0].height));
	draw_texture(ninePatchRect[6], xPos + (width * ninePatchRect[0].width), yPos);
	draw_texture(ninePatchRect[8], xPos + (width * ninePatchRect[0].width), yPos + (height * ninePatchRect[0].height));

	for (int x = 1; x < width; ++x) {
		draw_texture(ninePatchRect[3], (x * ninePatchRect[0].width) + xPos, yPos);
		draw_texture(ninePatchRect[5], (x * ninePatchRect[0].width) + xPos, yPos + (height * ninePatchRect[0].height));
	}
	for (int y = 1; y < height; ++y) {
		draw_texture(ninePatchRect[1], xPos, (y * ninePatchRect[0].height) + yPos);
		draw_texture(ninePatchRect[7], xPos + (width * ninePatchRect[0].width), (y * ninePatchRect[0].height) + yPos);
	}
	for (int x = 1; x < width; ++x) {
		for (int y = 1; y < height; ++y) {
			draw_texture(ninePatchRect[4], (x * ninePatchRect[0].width) + xPos, (y * ninePatchRect[0].height) + yPos);
		}
	}
}

void Menu::row(const u16 num_columns, Texture bg[9]) {
	assert(num_columns >= 0);
	assert(width >= 1);
	height = num_columns + 1;

	this->bg = bg;

	if (allignmentX == ALLIGN_CENTER) {
		xPos = get_virtual_width() / 2;
		xPos -= (((width + 1) * bg[0].width) / 2);
	}
	if (allignmentY == ALLIGN_CENTER) {
		yPos = get_virtual_height() / 2;
		yPos -= (((height + 1) * bg[0].height) / 2);
	}

	curr_x = xPos;
	curr_y = yPos + bg[0].height / 2;
}

bool Menu::push_button(const char* text, vec2 mousePos, Font font) {
	const u32 text_width = get_string_width(font, text);
	bool pressed = false;
	if (center)
		pressed = draw_text_button(text, text_width, (xPos + ((width + 1) * bg[0].width / 2)) - (text_width / 2), curr_y, mousePos, font);
	else
		pressed = draw_text_button(text, text_width, xPos + 6, curr_y, mousePos, font);
	curr_y += bg[0].height + padding;
	return pressed;
}

void Menu::push_label(const char* text, Font font) {
	const u32 text_width = get_string_width(font, text);
	if (center)
		draw_text(font, text, xPos - (text_width / 2), curr_y, TEXT_COLOR.x, TEXT_COLOR.y, TEXT_COLOR.z);
	else
		draw_text(font, text, xPos, curr_y, TEXT_COLOR.x, TEXT_COLOR.y, TEXT_COLOR.z);
	curr_y += bg[0].height + padding;
}

void Menu::push_divider() {
	draw_rectangle(xPos, curr_y + (bg[0].height / 2) - padding, bg[0].width * (width + 1), 1, TEXT_COLOR);
	curr_y += bg[0].height - padding;
}

void Menu::title(const char* const title, Font font) {
	const u32 title_width = get_string_width(font, title);
	curr_y = yPos + (bg[0].width / 2) - (font.characters['T']->texture.height);
	draw_text(font, title, xPos + (((width + 1) * bg[0].width) / 2) - (title_width / 2), curr_y);
	curr_y += font.characters['T']->texture.height;
}

/////////////////////////////////////////////////////////////////////////////

void Splash::begin(Texture tex, const double fade_speed) {
	this->fade_speed = fade_speed;
	this->tex = tex;
	alpha = 1;
	done = false;
}

bool Splash::draw() {
	if (!done) {
		if (alpha <= 0) {
			end();
			return true;
		}
		else {
			alpha += fade_speed;
			if (alpha >= 330) {
				alpha = 255;
				fade_speed = -fade_speed;
			}
			draw_texture(tex, xPos, yPos, 255, 255, 255, alpha);
		}
	}
	return false;
}

void Splash::end() {
	done = true;
	fade_speed = 0;
}

/////////////////////////////////////////////////////////////////////////////

Animation create_animation(Texture sheet, u8 numFrames, u8 frameWidth, u8 frameHeight, u8 row, u16 delay) {
	assert(row > 0);

	Animation anim;
	anim.animationSheet = sheet;
	anim.numFrames = numFrames;
	anim.frameWidth = frameWidth;
	anim.frameHeight = frameHeight;
	anim.row = row - 1;
	anim.delay = delay;
	anim.currentFrame = 0;
	anim.playing = false;
	anim.looping = false;
	anim.timer = 0;
	return anim;
}

void start_animation(Animation& animation, bool looping, u64 flipFlag) {
	animation.playing = true;
	animation.looping = looping;
	animation.currentFrame = 0;
	animation.flipFlag = flipFlag;
	animation.timer = 0;
}

void stop_animation(Animation& animation) {
	animation.playing = false;
	animation.timer = 0;
}

void draw_animation(Animation& animation, i32 xPos, i32 yPos) {
	if (animation.playing) {
		animation.timer++;
		if (animation.timer > animation.delay) {
			animation.timer = 0;

			animation.currentFrame++;
			if (animation.currentFrame >= animation.numFrames) {
				if (animation.looping) {
					animation.currentFrame = 0;
				}
				else
					stop_animation(animation);
			}
		}
		Rect src;
		src.x = animation.currentFrame * animation.frameWidth;
		src.y = animation.row * animation.frameWidth;
		src.width = animation.frameWidth;
		src.height = animation.frameHeight;

		Rect dest;
		dest.x = xPos;
		dest.y = yPos;
		dest.width = animation.frameWidth;
		dest.height = animation.frameHeight;

		u64 oldFlipFlag = animation.animationSheet.flip_flag;
		animation.animationSheet.flip_flag = animation.flipFlag;
		draw_texture_EX(animation.animationSheet, src, dest);
		animation.animationSheet.flip_flag = oldFlipFlag;
	}
}
