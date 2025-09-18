#include "Camera.h"

#include <VolcaniCore/Core/Assert.h>

#include "OrthographicCamera.h"
#include "StereographicCamera.h"

using namespace VolcaniCore;

namespace Magma::Graphics {

Ref<Camera> Camera::Create(Camera::Type type) {
	if(type == Camera::Type::Ortho) {
		return CreateRef<OrthographicCamera>();
	}
	return CreateRef<StereographicCamera>();
}

Ref<Camera> Camera::Create(Camera::Type type, float fovOrRotation) {
	if(type == Camera::Type::Ortho) {
		return CreateRef<OrthographicCamera>(fovOrRotation);
	}
	return CreateRef<StereographicCamera>(fovOrRotation);
}

Camera::Camera(Camera::Type type)
	: m_Type(type) { }

Camera::Camera(Camera::Type type, uint32_t width, uint32_t height,
				float near, float far)
	: m_Type(type), ViewportWidth(width), ViewportHeight(height),
		Near(near), Far(far)
{
	VOLCANICORE_ASSERT(width != 0 && height != 0,
						"Viewport width and height must not be 0");
	VOLCANICORE_ASSERT(near != 0 && far != 0,
						"Near and far clip must not be non-zero");
}

void Camera::Resize(uint32_t width, uint32_t height) {
	if(width == ViewportWidth && height == ViewportHeight)
		return;

	ViewportWidth = width ? width : ViewportWidth;
	ViewportHeight = height ? height : ViewportHeight;
	CalculateProjection();
}

void Camera::SetProjection(float near, float far) {
	if(near == Near && far == Far)
		return;

	Near = near ? near : Near;
	Far = far ? far : Far;
	CalculateProjection();
}

void Camera::SetPosition(const glm::vec3& position) {
	Position = position;
	CalculateView();
}

void Camera::SetDirection(const glm::vec3& direction) {
	Direction = glm::normalize(direction);
	CalculateView();
}

void Camera::SetPositionDirection(const glm::vec3& pos, const glm::vec3& dir) {
	Position = pos;
	Direction = dir;
	CalculateView();
}

}