#ifndef GLOBALS_H
#define GLOBALS_H

#include <texture.h>
#include <maths.h>
#include <bahamut.h>
#include <time.h>
#include <vector>
#include <random>
#include <string>

#define SCROLL_SPEED 5;
#define TILE_SIZE 32
#define internal static
#define global static
#define local static

struct Config {
	i32 windowWidth;
	i32 windowHeight;
	bool fullscreen;
	bool resizable;
	bool compressSaves;
	bool soundOn;
	bool monitor;
	bool vsync;
	bool showFPS;
	u8 musicVolume;
	u8 soundVolume;
	u8 masterVolume;
	i32 texParam;
	Font header;
	u32 headerSize;
	Font body;
	u32 bodySize;
	u32 fpsCap;
	u8 priority;
	char* headerFontPath;
	char* bodyFontPath;
};

typedef std::vector<std::string> StringList;

internal inline
StringList split_string(char* str, char seperator) {
	StringList result;
	do {
		char* begin = str;
		while(*str != seperator && *str)
			str++;
		result.push_back(std::string(begin, str));
	} while (0 != *str++);
	return result;
}

internal inline
bool is_a_number(char c) {
	return (c > 47 && c < 58);
}

internal inline
void remove_characters(char* string, char to_remove) {
	char* i = string;
	char* j = string;
	while (*j != NULL) {
		*i = *j++;
		if (*i != to_remove) i++;
	}
	*i = 0;
}

internal inline
bool character_exists(char* string, char to_check) {
	int len = strlen(string);
	for (int i = 0; i < len; ++i)
		if (string[i] == to_check) return true;
	return false;
}

internal inline
StringList decipher_line(char* line) {
	StringList tokens;
	remove_characters(line, '\n');
	remove_characters(line, '\t');
	remove_characters(line, ' ');
	if (line[0] == '(') {
		remove_characters(line, '(');
		remove_characters(line, ')');
		tokens = split_string(line, '=');
		if (tokens[0].empty()) {
			if (tokens.size() > 1)
				BMT_LOG(FATAL_ERROR, "[decipher_line] No identifier in parenthesis.");
			else
				BMT_LOG(FATAL_ERROR, "[decipher_line] Parenthesis is empty.");
		}

		else if (tokens.size() == 1) 
			return tokens;
									
		else if (tokens.size() > 2)
			BMT_LOG(FATAL_ERROR, "[decipher_line] Multiple '=' present inside parenthesis.");

		else {
			StringList parameters;
			char* second_part = (char*)malloc(tokens[1].size() + 1);
			for (u32 i = 0; i < tokens[1].size(); ++i)
				second_part[i] = tokens[1].at(i);
			second_part[tokens[1].size()] = 0;

			if (character_exists(second_part, ',')) {
				tokens.pop_back();
				parameters = split_string(second_part, ',');
				for (u32 i = 0; i < parameters.size(); ++i)
					tokens.push_back(parameters.at(i));
			}
			free(second_part);
		}
	}
	return tokens;
}

#define ERR_ARG(NAME) \
	if(tokens.size() > 2) \
		BMT_LOG(FATAL_ERROR, #NAME " should only have one argument"); \

#define INIT_BOOL(NAME, VAR) \
	if(tokens[0] == #NAME) { \
		ERR_ARG(NAME) \
		if(tokens[1] == "YES") \
			VAR = true; \
		else if(tokens[1] == "NO") \
			VAR = false; \
		else \
			BMT_LOG(FATAL_ERROR, #NAME " can only equal YES or NO"); \
		return; \
	}

#define INIT_NUMBER(NAME, VAR) \
	if(tokens[0] == #NAME) { \
		ERR_ARG(NAME) \
		if(is_a_number(tokens[1].at(0))) \
			VAR = std::stoi(tokens[1]); \
		else \
			BMT_LOG(FATAL_ERROR, #NAME " must be a whole number"); \
		return; \
	}

internal inline
void process_config_line(StringList& tokens, Config* con) {
	if(tokens[0] == "TEXTURE_SCALING") {
		ERR_ARG(con->texParam)
		if(tokens[1] == "NEAREST")
			con->texParam = GL_NEAREST;
		else if(tokens[1] == "LINEAR")
			con->texParam = GL_LINEAR;
		else
			BMT_LOG(FATAL_ERROR, "TEXTURE_SCALING must be NEAREST or LINEAR");
	}

	if (tokens[0] == "BODY_FONT") {
		std::string path = "data/art/";
		path.append(tokens[1]);
		con->bodyFontPath = (char*)malloc(path.size() + 1);
		for (int i = 0; i < path.size(); ++i)
			con->bodyFontPath[i] = path[i];
		con->bodyFontPath[path.size()] = 0;
	}

	if (tokens[0] == "HEADER_FONT") {
		std::string path = "data/art/";
		path.append(tokens[1]);
		con->headerFontPath = (char*)malloc(path.size() + 1);
		for (int i = 0; i < path.size(); ++i)
			con->headerFontPath[i] = path[i];
		con->headerFontPath[path.size()] = 0;
	}

	INIT_BOOL(SOUND, con->soundOn)
	INIT_BOOL(FULLSCREEN, con->fullscreen)
	INIT_BOOL(RESIZABLE, con->resizable)
	INIT_BOOL(DISPLAY_FPS, con->showFPS)
	INIT_BOOL(COMPRESS_SAVES, con->compressSaves)
	INIT_BOOL(VSYNC, con->vsync)
#undef INIT_BOOL

	INIT_NUMBER(WINDOW_WIDTH, con->windowWidth)
	INIT_NUMBER(WINDOW_HEIGHT, con->windowHeight)
	INIT_NUMBER(BODY_FONT_SIZE, con->bodySize)
	INIT_NUMBER(HEADER_FONT_SIZE, con->headerSize)
	INIT_NUMBER(FPS_CAP, con->fpsCap)
	INIT_NUMBER(SOUND_VOLUME, con->soundVolume)
	INIT_NUMBER(MUSIC_VOLUME, con->musicVolume)
	INIT_NUMBER(MASTER_VOLUME, con->masterVolume)
#undef INIT_NUMBER
#undef ERR_ARG
}

internal inline
Texture get_sub_image(unsigned char* pixels, int pixels_width, int x, int y, int width, int height, int texparam) {
	Texture subimage;
	glPixelStorei(GL_UNPACK_ROW_LENGTH, pixels_width);
	unsigned char* subimage_pixels = pixels + (x + y * pixels_width) * 4;
	subimage = load_texture(subimage_pixels, width, height, texparam);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	return subimage;
}

internal inline
Config load_config(const char* filename) {
	Config con = {0};
	BMT_LOG(INFO, "[%s] Loading config data", filename);
	char buffer[255];
	FILE* in = fopen(filename, "r");
	if(in == NULL) BMT_LOG(FATAL_ERROR, "[%s] Could not open file.", filename);

	i32 i = 0;
	while(true) {
		i++;
		buffer[0] = 0;
		fgets(buffer, 255, in);
		if(buffer[0] == 0) break;
		StringList tokens = decipher_line(buffer);

		if(tokens.size() > 0)
			process_config_line(tokens, &con);
	}
	fclose(in);
	return con;
}

internal inline
Font test_load_font(const GLchar* filepath, unsigned int size, i32 texParam) {
	Font font = { 0 };
	font.size = size;
	//load lib
	if (FT_Init_FreeType(&font.ft)) {
		printf("Could not initialize FreeType font\n");
		font = { 0 };
		return font;
	}

	//load face
	if (FT_New_Face(font.ft, filepath, 0, &font.face)) {
		printf("Failed to load font '%s'\n", filepath);
		font = { 0 };
		return font;
	}
	FT_Set_Pixel_Sizes(font.face, 0, size);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//store char textures
	for (GLubyte c = 32; c < 128; ++c) {
		if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) {
			printf("Failed to load Glyph '%c'\n", c);
			continue;
		}

		Character* character = new Character;

		glGenTextures(1, &character->texture.ID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, character->texture.ID);

		character->texture.width = font.face->glyph->bitmap.width;
		character->texture.height = font.face->glyph->bitmap.rows;
		character->texture.flip_flag = 0;

		GLubyte* glyphPixels = font.face->glyph->bitmap.buffer;

		//convert the pixels from alpha only to RGBA (set RGB to white, then A to glyphPixels)
		GLubyte* pixels = (GLubyte*)malloc(character->texture.width * character->texture.height * 4);
		if (glyphPixels != NULL) {
			int j = 0;
			for (int i = 0; i < character->texture.width * character->texture.height * 4; ++i) {
				pixels[i++] = 255; //red
				pixels[i++] = 255; //green
				pixels[i++] = 255; //blue
				pixels[i] = glyphPixels[j++]; //alpha
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, character->texture.width, character->texture.height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pixels
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texParam);
		glBindTexture(GL_TEXTURE_2D, 0);

		character->size = V2((float)font.face->glyph->bitmap.width, (float)font.face->glyph->bitmap.rows);
		character->bearing = V2((float)font.face->glyph->bitmap_left, (float)font.face->glyph->bitmap_top);
		character->advance = font.face->glyph->advance.x;

		free(pixels);

		font.characters[c] = character;
	}

	//reset gl_unpack_alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return font;
}

internal inline
void init_context(const char* title, Config* con) {
	init_window(con->windowWidth, con->windowHeight, title, con->fullscreen, con->resizable, true);
	init_audio();
	srand(time(NULL));

	con->body = load_font(con->bodyFontPath, con->bodySize);
	con->header = load_font(con->headerFontPath, con->headerSize);
	free(con->bodyFontPath);
	free(con->headerFontPath);

	set_FPS_cap(con->fpsCap);
	set_vsync(con->vsync);
	set_master_volume(con->masterVolume);

	//TILE_SIZE *= SCALE;

	set_clear_color(0, 0, 0, 255);
}

internal inline
i32 random_int(i32 min, i32 max) {
	i32 maxMin = (max - min);
	if (maxMin == 0) return 0; //don't divide by 0
	return (rand() % maxMin) + min;
}

#endif