#pragma once

#include "Camera.h"

using namespace VolcaniCore;

namespace Magma::Graphics {

class OrthographicCamera : public Camera {
public:
	OrthographicCamera();
	OrthographicCamera(float rotation);
	OrthographicCamera(uint32_t width, uint32_t height, float near, float far,
						float rotation = 0.0f);
	~OrthographicCamera() = default;

	void SetRotation(float rotation);
	float GetRotation() const { return m_Rotation; }

private:
	float m_Rotation;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}