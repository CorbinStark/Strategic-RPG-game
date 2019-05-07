#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "states.h"

class MenuState : public State {
public:
	//constructor
	MenuState();
	//called when state is switched to
	void enter();
	//called every frame
	void update(vec2 mousePos);
	//called when state is switched from
	void leave();
	//destructor
	~MenuState();
private:
	void draw();

	//Texture test;
	//Texture button;
	Font largefont;

	Model testmodel;

	mat4 biasMatrix;
	Framebuffer buffer;

	Shader basic3D;
	Shader shadowShader;
	Shader TEST_SHADOWS;

	vec3 lightInvDir;
	mat4 depthProjection;
	mat4 depthView;
	mat4 depthMVP;
	mat4 depthBiasMVP;
};

#endif