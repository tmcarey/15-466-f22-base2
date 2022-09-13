#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint tower_meshes_for_lit_color_texture_program = 0;
GLuint disk_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > tower_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("tower.pnct"));
	tower_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});
Load< MeshBuffer > disk_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("disk.pnct"));
	disk_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > tower_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("tower.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = tower_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = tower_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*tower_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Tower") tower = &transform;
		if (transform.name == "Pointer") pointer = &transform;
	}
	pointer->scale = glm::vec3(0.1f, 0.1f, 0.1f);


	Mesh const &disk_mesh = disk_meshes->lookup("Disk");
	Mesh const &disk_outline_mesh = disk_meshes->lookup("DiskOutline");
	for (int i = 0; i < 1; i++){
		Scene::Transform *t = new Scene::Transform();
		t->parent = tower;
		t->position = glm::vec3(0, 0.0f, 1.0f + i);
		scene.drawables.emplace_back(t);
		Scene::Drawable &drawable = scene.drawables.back();
		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = disk_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = disk_mesh.type;
		drawable.pipeline.start = disk_mesh.start;
		drawable.pipeline.count = disk_mesh.count;

		Scene::Transform *outline_t = new Scene::Transform();
		outline_t->parent = t;
		scene.drawables.emplace_back(outline_t);
		Scene::Drawable &drawableOutline = scene.drawables.back();
		drawableOutline.pipeline = lit_color_texture_program_pipeline;
		drawableOutline.pipeline.vao = disk_meshes_for_lit_color_texture_program;
		drawableOutline.pipeline.type = disk_outline_mesh.type;
		drawableOutline.pipeline.start = disk_outline_mesh.start;
		drawableOutline.pipeline.count = disk_outline_mesh.count;

		disks.push_back(Disk(t, outline_t, BoxCollider(glm::vec3(1.0f, 1.0f, 0.5f), t), 0, 0));
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
		if(heldDisk == nullptr && targetedDisk != nullptr){
			heldDisk = targetedDisk;
			targetedDisk->diskOutline->enabled = false;
			heldDisk->disk->parent = nullptr;
		}else if(heldDisk != nullptr){
			heldDisk = nullptr;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			mousePosition = glm::vec2(
				evt.motion.x / float(window_size.y),
				-evt.motion.y / float(window_size.y)
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	glm::vec3 mousePoint00(-7.0, -20.0f, 8.0f);
	glm::vec3 horizMouse(7.8f, 0, 0);
	glm::vec3 vertMouse(0, 0.0f, 7.0f);

	pointer->position = mousePoint00 + horizMouse * mousePosition.x + vertMouse * mousePosition.y; 

	glm::vec3 origin = camera->transform->position;
	glm::vec3 toPointer = pointer->position - origin;
	glm::vec3 dir(toPointer.x / toPointer.length(), 
			toPointer.y / toPointer.length(),
			toPointer.z / toPointer.length());

	if(heldDisk == nullptr){
		float minT = 100000.f;
		Disk *targeted = nullptr;
		for(auto it = disks.begin(); it < disks.end(); it++){
			float t = 0;
			if(it->collider.RayBoxIntersect(origin, toPointer, &t)){
				if(t < minT){
					minT = t;
					it->diskOutline->enabled = true;
					if(targeted){
						targeted->diskOutline->enabled = false;
					}
					targeted = &(*it);
				}
			}else{
				it->diskOutline->enabled = false;
			}
		}
		targetedDisk = targeted;
	}else{
		if(dir.y == 0){
			dir.y = 0.000001f;
		}
		heldDisk->disk->position = origin + (dir * (origin.y / -dir.y));
	}




	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
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

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
