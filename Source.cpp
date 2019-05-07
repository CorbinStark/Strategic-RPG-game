#include <bahamut.h>
#include <random>

#include "level.h"

void load_scene(MapScene* scene);

int main() {
	init_window(800, 600, "Space RTS", false, true, 1);
	init_audio();

	set_FPS_cap(60);
	set_vsync(false);
	set_master_volume(255);
	set_clear_color(0.2f * 255, 0.8f * 255, 0.9f * 255, 255);
	set_mouse_locked(true);

	Shader basic = load_default_shader_2D();
	Shader basic3D = load_shader_3D("data/vertshader.txt", "data/fragshader.txt");

	Model boat = load_model("data/palm_short.obj", BROWN);
	//boat.texture = load_texture("data/gravetex.png", GL_LINEAR);
	Model boat2 = load_model("data/ship_light.obj", BROWN);

	Texture cursor = load_texture("data/art/cursor.png", GL_NEAREST);
	//boat.texture = cursor;

	MapScene scene = { 0 };
	load_scene(&scene);
	Map map = { 0 };
	Unit test = { 0 };
	push_unit(test, &map.units);
	test.pos.x = random_int(0, 800);
	test.pos.y = random_int(0, 600);
	test.shipType = 3;
	test.owner = OWNER_BLUE;
	push_unit(test, &map.units);
	test.pos.x = random_int(0, 800);
	test.pos.y = random_int(0, 600);
	test.shipType = 2;
	test.owner = OWNER_GREEN;
	push_unit(test, &map.units);
	test.pos.x = random_int(0, 800);
	test.pos.y = random_int(0, 600);
	test.owner = OWNER_RED;
	push_unit(test, &map.units);
	test.pos.x = random_int(0, 800);
	test.pos.y = random_int(0, 600);
	test.shipType = 5;
	test.velocity.x = 200;
	push_unit(test, &map.units);

	vec2 lastMousePos = { 0 };
	Camera cam = { 0 };

	while (window_open()) {
		vec2 mousePos = get_mouse_pos();
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

		begin_drawing();
		set_viewport(0, 0, get_window_width(), get_window_height());

		////map
		//begin2D(basic);
		//	upload_mat4(basic, "projection", orthographic_projection(0, 0, get_window_width() * SCALE, get_window_height() * SCALE, -1, 1));
		//	draw_background(&scene);
		//	draw_map(&scene, &map);
		//	map_input(&map);
		//end2D();
		//
		////ui
		//begin2D(basic);
		//	upload_mat4(basic, "projection", orthographic_projection(0, 0, get_window_width(), get_window_height(), -1, 1));
		//	draw_texture(cursor, mousePos.x, mousePos.y);
		//end2D();

		begin3D(basic3D);
			boat.pos.z -= 0.01f;
			boat.rotate.x -= 0.1f;
			boat.rotate.y -= 0.1f;
			boat.rotate.z -= 0.1f;
			boat2.pos.z += 0.01f;
			boat2.rotate.x -= 0.1f;
			boat2.rotate.y -= 0.1f;
			boat2.rotate.z -= 0.1f;
			upload_mat4(basic3D, "projection", perspective_projection(90, 1.777778f, 0.1f, 999.9f));
			upload_mat4(basic3D, "view", create_view_matrix(cam));
			upload_vec3(basic3D, "viewPos", V3(cam.x, cam.y, cam.z));
			draw_cube(V3(0, 0, 0), V3(1, 1, 1), V3(0, 90, 0), RED);
			draw_model(boat);
			draw_model(boat2);
		end3D();
		end_drawing();
	}

	dispose_texture(cursor);
	dispose_window();

	return 0;
}

void load_scene(MapScene* scene) {
	scene->shipSets[0].ships[0] = load_texture("data/art/ShipBlack1.png", GL_NEAREST);
	scene->shipSets[0].ships[1] = load_texture("data/art/ShipBlack2.png", GL_NEAREST);
	scene->shipSets[0].ships[2] = load_texture("data/art/ShipBlack3.png", GL_NEAREST);
	scene->shipSets[0].ships[3] = load_texture("data/art/ShipBlack4.png", GL_NEAREST);
	scene->shipSets[0].ships[4] = load_texture("data/art/ShipBlack5.png", GL_NEAREST);
	scene->shipSets[0].ships[5] = load_texture("data/art/ShipBlack6.png", GL_NEAREST);

	scene->shipSets[1].ships[0] = load_texture("data/art/ShipBlue1.png", GL_NEAREST);
	scene->shipSets[1].ships[1] = load_texture("data/art/ShipBlue2.png", GL_NEAREST);
	scene->shipSets[1].ships[2] = load_texture("data/art/ShipBlue3.png", GL_NEAREST);
	scene->shipSets[1].ships[3] = load_texture("data/art/ShipBlue4.png", GL_NEAREST);
	scene->shipSets[1].ships[4] = load_texture("data/art/ShipBlue5.png", GL_NEAREST);
	scene->shipSets[1].ships[5] = load_texture("data/art/ShipBlue6.png", GL_NEAREST);

	scene->shipSets[2].ships[0] = load_texture("data/art/ShipGreen1.png", GL_NEAREST);
	scene->shipSets[2].ships[1] = load_texture("data/art/ShipGreen2.png", GL_NEAREST);
	scene->shipSets[2].ships[2] = load_texture("data/art/ShipGreen3.png", GL_NEAREST);
	scene->shipSets[2].ships[3] = load_texture("data/art/ShipGreen4.png", GL_NEAREST);
	scene->shipSets[2].ships[4] = load_texture("data/art/ShipGreen5.png", GL_NEAREST);
	scene->shipSets[2].ships[5] = load_texture("data/art/ShipGreen6.png", GL_NEAREST);

	scene->shipSets[3].ships[0] = load_texture("data/art/ShipRed1.png", GL_NEAREST);
	scene->shipSets[3].ships[1] = load_texture("data/art/ShipRed2.png", GL_NEAREST);
	scene->shipSets[3].ships[2] = load_texture("data/art/ShipRed3.png", GL_NEAREST);
	scene->shipSets[3].ships[3] = load_texture("data/art/ShipRed4.png", GL_NEAREST);
	scene->shipSets[3].ships[4] = load_texture("data/art/ShipRed5.png", GL_NEAREST);
	scene->shipSets[3].ships[5] = load_texture("data/art/ShipRed6.png", GL_NEAREST);

	scene->asteroid1 = load_texture("data/art/asteroid1.png", GL_NEAREST);
	scene->asteroid2 = load_texture("data/art/asteroid2.png", GL_NEAREST);
	scene->asteroid3 = load_texture("data/art/asteroid3.png", GL_NEAREST);

	scene->bg = load_texture("data/art/background.png", GL_NEAREST);
	scene->station = load_texture("data/art/SpaceStation.png", GL_NEAREST);
	scene->selection = load_texture("data/art/selection.png", GL_NEAREST);
	scene->destarrow = load_texture("data/art/destarrow.png", GL_NEAREST);

	scene->bodyFont = load_font("data/art/pixantiqua.ttf", 12);
}