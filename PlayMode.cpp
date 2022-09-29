#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

Load< Scene > blank_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("blank_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){

	});
});

Load< Sound::Sample > wildflowers_music_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("wildflower_reserve_demo.opus"));
});
Load< Sound::Sample > prayers_music_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("prayers_demo.opus"));
});
Load< Sound::Sample > hudson_music_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("hudson_demo.opus"));
});

PlayMode::PlayMode() : scene(*blank_scene) {

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	// Setup music loops
	cur_loop = Sound::loop(*wildflowers_music_sample);

	SetupPoems();

	// Set up text renderers
	display_text = new TextRenderer(data_path("EBGaramond-Regular.ttf").c_str(), display_font_size);
	body_text = new TextRenderer(data_path("Overlock-Regular.ttf").c_str(), body_font_size);
}

PlayMode::~PlayMode() {
	delete display_text;
	delete body_text;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_RETURN) {
			enter.downs += 1;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_1) {
			one.downs += 1;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_2) {
			two.downs += 1;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		lmb.downs += 1;
	}

	return false;
}

void PlayMode::update(float elapsed) {

	timer += elapsed;

	if (finished_poem) {
		if (cur_poem.left && one.downs == 1) {
			cur_poem = *cur_poem.left;
			waiting = false;
			cur_index = 0;
			cur_line = 0;
			cur_string = "";
			finished_poem = false;
			SetMusic();
		}
		else if (cur_poem.right && two.downs == 1) {
			cur_poem = *cur_poem.right;
			waiting = false;
			cur_index = 0;
			cur_line = 0;
			cur_string = "";
			finished_poem = false;
			SetMusic();
		}
	}

	if (waiting && (lmb.downs == 1 || space.downs == 1 || enter.downs == 1)) {
		waiting = false;
		cur_index = 0;
		cur_line++;
		cur_string = "";
	}

	if (timer >= tick_rate) {
		timer = timer - tick_rate;
		
		if (!waiting) {
			if (cur_index < cur_poem.lines[cur_line].length()) {
				cur_string += cur_poem.lines[cur_line][cur_index];
				cur_index++;
			}
			else if (cur_line < cur_poem.lines.size() - 1) {
				waiting = true;
			}
			else {
				finished_poem = true;
			}
		}
	}


	//reset button press counters:
	enter.downs = 0;
	space.downs = 0;
	lmb.downs = 0;
	one.downs = 0;
	two.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	glDisable(GL_DEPTH_TEST);
	
	// Draw title
	display_text->draw(cur_poem.title.c_str(), -10.0f, 2.5f, 1.0f, glm::vec3(0.64f, 0.5f, 0.95f), float(drawable_size.x), float(drawable_size.y));
	display_text->draw(cur_poem.title.c_str(), -20.0f, -1.5f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), float(drawable_size.x), float(drawable_size.y));
	

	// Draw current line or input prompt
	if (cur_line == -1) {
		body_text->draw("[click]", x_anchor * float(drawable_size.x), (y_anchor * float(drawable_size.y)), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), float(drawable_size.x), float(drawable_size.y));
	}
	else {
		body_text->draw(cur_string.c_str(), x_anchor * float(drawable_size.x), (y_anchor * float(drawable_size.y)), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), float(drawable_size.x), float(drawable_size.y));
	}

	// Draw past lines
	for (uint16_t i = 0; i < cur_line; i++) {
		body_text->draw(
			cur_poem.lines[i].c_str(),
			x_anchor * float(drawable_size.x),
			(y_anchor * float(drawable_size.y)) + ((cur_line - i) * body_font_size) + line_spacing,
			1.0f, 
			glm::vec3(1.0f, 1.0f, 1.0f), 
			float(drawable_size.x), float(drawable_size.y));
	}

	if (finished_poem) {
		if (cur_poem.left) {
			body_text->draw((cur_poem.left->verb + " [1]").c_str(), 10.0f, (0.5f * float(drawable_size.y)), 1.0f, glm::vec3(0.7f, 0.7f, 0.7f), float(drawable_size.x), float(drawable_size.y));
		}
		if (cur_poem.right) {
			body_text->draw((cur_poem.right->verb + " [2]").c_str(), float(drawable_size.x) - 100.0f, (0.5f * float(drawable_size.y)), 1.0f, glm::vec3(0.7f, 0.7f, 0.7f), float(drawable_size.x), float(drawable_size.y));
		}
	}

	GL_ERRORS();
}

// bad
void PlayMode::SetMusic() {
	if (cur_poem.title == wildflowers.title) {
		cur_loop->stop();
		cur_loop = Sound::loop(*wildflowers_music_sample);
	}
	else if (cur_poem.title == prayers.title) {
		cur_loop->stop();
		cur_loop = Sound::loop(*prayers_music_sample);
	}
	else {
		cur_loop->stop();
		cur_loop = Sound::loop(*hudson_music_sample);
	}
}
