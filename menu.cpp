#include "menu.h"

#include <stdio.h>
#include <time.h>

//constructor
MenuState::MenuState() {
	biasMatrix =
	{
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};
//	buffer = create_framebuffer(600, 400, TEXTURE_PARAM, DEPTHBUFFER);
	buffer.texture.flip_flag = FLIP_VERTICAL;
	
	//depthProjection = orthographic_projection(-10, 10, -10, 10, -10, 10);
	vec3 lightPos = V3(40, 20, 10);

	depthProjection = perspective_projection(90, 1.7778f, 0.1f, 999.9f);
	depthView = look_at(lightPos, V3(0, 0, 0));
	depthMVP = depthProjection * depthView;
	depthBiasMVP = biasMatrix * depthMVP;

	shadowShader = load_shader_3D("data/shaders/SHADOWMAP_VERT.txt", "data/shaders/SHADOWMAP_FRAG.txt");
	begin3D(shadowShader);
		upload_mat4(shadowShader, "projection", depthProjection);
		upload_mat4(shadowShader, "view", depthView);
	end3D();

	basic3D = load_shader_3D("data/shaders/SHADOW_BASIC_VERT.txt", "data/shaders/SHADOW_BASIC_FRAG.txt");
	begin3D(basic3D);
		upload_mat4(basic3D, "projection", perspective_projection(90, 1.7778f, 0.1f, 999.9f));
		upload_mat4(basic3D, "depthMVP", depthMVP);
		upload_vec3(basic3D, "lightPos", lightPos);
	end3D();

	TEST_SHADOWS = load_shader_2D("data/shaders/TEST_SHADOWMAP_VERT.txt", "data/shaders/TEST_SHADOWMAP_FRAG.txt");
	begin2D(TEST_SHADOWS);
		upload_float(TEST_SHADOWS, "near_plane", 0.1f);
		upload_float(TEST_SHADOWS, "far_plane", 999.9f);
		upload_mat4(TEST_SHADOWS, "projection", orthographic_projection(0, 0, get_window_width(), get_window_height(), -10, 10));
	end2D();
}

//called when state is switched to
void MenuState::enter() {
	//test = load_texture("data/art/deskbg_light.png", TEXTURE_PARAM);
	//largefont = load_font("data/art/NotoSans-Regular.ttf", 56);
}

void MenuState::draw() {
	STORAGE f32 ROTATE_TEST = 0;

	draw_cube(V3(0, -5, 5), V3(15, 1, 15), V3(0, 0, 0), GRAY);
	//draw_cube(V3(0, 0, 10), V3(1, 2, 1), V3(ROTATE_TEST, ROTATE_TEST++, ROTATE_TEST), test);
	for (int i = 0; i < 10; i++) {
		//draw_cube(V3((i * 2) - 7, 0, -6), V3(1, 1, 1), V3(0, 0, 0), test);
	}
}

//called every frame
void MenuState::update(vec2 mousePos) {
	STORAGE vec2 lastMousePos = { 0 };
	STORAGE Camera cam = { 0 };

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cam.yaw += (mousePos.x - lastMousePos.x) * 0.2f;
	cam.pitch += (mousePos.y - lastMousePos.y) * 0.2f;
	lastMousePos = mousePos;

	if (is_key_down(KEY_A)) {
		move_cam_left(&cam, 1);
	}
	if (is_key_down(KEY_D)) {
		move_cam_right(&cam, 1);
	}
	if (is_key_down(KEY_S)) {
		move_cam_backwards(&cam, 1);
	}
	if (is_key_down(KEY_W)) {
		move_cam_forward(&cam, 1);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	set_viewport(0, 0, buffer.texture.width, buffer.texture.height);
	bind_framebuffer(buffer);
	clear_bound_framebuffer();
	begin3D(shadowShader);
		glCullFace(GL_FRONT);
		draw();
		glCullFace(GL_BACK);
	end3D();
	unbind_framebuffer();

	set_viewport(0, 0, get_window_width(), get_window_height());
	begin3D(basic3D);
		bind_texture(buffer.texture, 1);
		upload_int(basic3D, "shadowMap", 1);
		upload_mat4(basic3D, "view", create_view_matrix(cam));
		draw();
	end3D();

	STORAGE bool showDepthBuffer = false;
	if (is_key_released('`')) showDepthBuffer = !showDepthBuffer;

	if (showDepthBuffer) {
		begin2D(TEST_SHADOWS);
		draw_framebuffer(buffer, 0, 0);
		end2D();
	}
}

//called when state is switched from
void MenuState::leave() {
	dispose_font(largefont);
}

//destructor
MenuState::~MenuState() {

}