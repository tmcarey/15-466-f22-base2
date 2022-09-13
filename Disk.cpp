#include "Disk.hpp"

Disk::Disk(Scene::Transform *disk,
			Scene::Transform *diskOutline,
			BoxCollider box,
			int towerId,
			int size) :
				disk(disk), diskOutline(diskOutline), collider(box), towerId(towerId), size(size) {
					diskOutline->enabled = false;
}
