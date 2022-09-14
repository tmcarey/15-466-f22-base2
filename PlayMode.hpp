#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include "Disk.hpp"

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform **tower_transforms[3];
	Scene::Transform *pointer = nullptr;
	/* Scene::Transform *upper_leg = nullptr; */
	/* Scene::Transform *lower_leg = nullptr; */
	/* glm::quat hip_base_rotation; */
	/* glm::quat upper_leg_base_rotation; */
	/* glm::quat lower_leg_base_rotation; */
	float wobble = 0.0f;
	std::list<Disk> tower1;
	std::list<Disk> tower2;
	std::list<Disk> tower3;
	glm::vec2 mousePosition;
	Disk *heldDisk = nullptr;
	Disk *targetedDisk = nullptr;
	
	//camera:
	Scene::Camera *camera = nullptr;
};
