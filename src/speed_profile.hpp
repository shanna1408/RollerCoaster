#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace modelling {
	class SpeedProfile {
        public:
            SpeedProfile() = default;
		    explicit SpeedProfile(float h, float s, float l);

            float getH() const;
            float getVmin() const;
            float getSpeed(float s, glm::vec3 curve_p, float v);

        private:
            float H, arcLength, topS, vdec;
            float g = 9.81f;
            float vmin = 2.5f;
            bool decel = false;

            float liftingPhase() const;
            float freefallPhase(float s, glm::vec3 curve_p) const;
            float deceleration(float s, float vdec) const;
    };
}