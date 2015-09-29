#include "Camera.h"

Camera::Camera(){}

void Camera::lookAt(const glm::vec3& camera, const glm::vec3& target)
{
	setPosition(camera);
	setOrientation(target - camera);
}

glm::vec3 Camera::getPosition() const
{
	return -1.0f * Transformable::getPosition();
}

void Camera::setPosition(const glm::vec3& pos, bool useScale)
{
	Transformable::setPosition(-1.0f*pos, useScale);
}

void Camera::setOrientation(const glm::vec3& orientation)
{
	glm::vec3 unit = glm::normalize(orientation);
	float longitude = 0.0f;
	float latitude  = acos(unit[1]);

	if(unit[2] == 0)
	{
		if(unit[0] < 0)
			longitude = PI/2;
		else
			longitude = -PI/2;
	}
	else
		longitude = atan(unit[0] / unit[2]);
	setRotate(latitude, glm::vec3(1.0f, 0.0f, 0.0f));
	setRotate(longitude, glm::vec3(0.0f, 1.0f, 0.0f));
}
