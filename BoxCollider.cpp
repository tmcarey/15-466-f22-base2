#include "BoxCollider.hpp"

BoxCollider::BoxCollider(glm::vec3 _dims, Scene::Transform *transform) : dims(_dims), transform(transform) {
	utmin = glm::vec3(-_dims.x,  -_dims.y, -_dims.z);
	utmax = glm::vec3(_dims.x, _dims.y, _dims.z);
}

bool BoxCollider::RayBoxIntersect(glm::vec3 origin, glm::vec3 dir, float *t){
	glm::vec3 min = utmin + transform->position;
	glm::vec3 max = utmax + transform->position;

	float tmin = (min.x - origin.x) / dir.x; 
    float tmax = (max.x - origin.x) / dir.x; 
 
    if (tmin > tmax) std::swap(tmin, tmax); 

    float tymin = (min.y - origin.y) / dir.y; 
    float tymax = (max.y - origin.y) / dir.y; 
 
    if (tymin > tymax) std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (min.z - origin.z) / dir.z; 
    float tzmax = (max.z - origin.z) / dir.z; 
 
    if (tzmin > tzmax) std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
	if(t){
		*t = tmin;
	}
    return true;
}
