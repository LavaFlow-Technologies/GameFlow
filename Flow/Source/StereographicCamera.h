#pragma once

#include <cstdint>

#include "Camera.h"

using namespace VolcaniCore;

namespace Magma::Graphics {

class StereographicCamera : public Camera {
public:
	StereographicCamera();
	StereographicCamera(float verticalFOV);
	StereographicCamera(float verticalFOV,
						uint32_t width, uint32_t height, float near, float far);
	~StereographicCamera() = default;

	void SetVerticalFOV(float verticalFOV);
	float GetVerticalFOV() const { return m_VerticalFOV; }

private:
	float m_VerticalFOV = 45.0f;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}