#include "glm/glm.hpp"

#include "Scene.hpp"

struct BoxCollider {
	BoxCollider(glm::vec3 dims, Scene::Transform *transform);
	bool RayBoxIntersect(glm::vec3 origin, glm::vec3 direction, float *t);

	private:
		Scene::Transform *transform;
		glm::vec3 dims;
		glm::vec3 utmin;
		glm::vec3 utmax;
};
