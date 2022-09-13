#include "Scene.hpp"

#include "BoxCollider.hpp"

struct Disk {
	Disk(Scene::Transform *disk,
			Scene::Transform *outline,
			BoxCollider coll,
			int towerId,
			int size);

	int size;
	int towerId;
	Scene::Transform *disk = nullptr;
	Scene::Transform *diskOutline = nullptr;
	BoxCollider collider;

};
