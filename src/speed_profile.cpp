#include "speed_profile.hpp"
#include <iostream>

namespace modelling {
    SpeedProfile::SpeedProfile(float h, float s, float l) : H(h), topS(s), arcLength(l) { 
        assert(h > 0.f); 
        assert(s > 0.f); 
        assert(l > 0.f); 
    }

    float SpeedProfile::getH() const { return H; }

    float SpeedProfile::getVmin() const { return vmin; }

    float SpeedProfile::liftingPhase() const { 
        std::cout << "lifting" << std::endl;
        return vmin; }

    float SpeedProfile::freefallPhase(float s, glm::vec3 curve_p) const { 
        std::cout << "freefall" << std::endl;
        return sqrt(2.f*g*(H-curve_p[1]));
    }

    float SpeedProfile::deceleration(float s, float vdec) const {
        std::cout << "deceleration" << std::endl;
        float ddec = arcLength - s;
        float Ldec = arcLength * 0.05;
        float v = (vdec - 0) * (ddec / Ldec);
        return v;
    }

    float SpeedProfile::getSpeed(float s, glm::vec3 curve_p, float v) { 
        if ((s/arcLength) > 0.95) {
            // If just entering deceleration phase, update vdec
            if (!decel) {
                decel = true;
                vdec = v;
            } 
            return deceleration(s, vdec);
        }

        decel = false;
        // If current position is before highest point of curve, use constant v
        if (s <= topS) {
            return liftingPhase();
        // Otherwise, enter freefall
        } else {
            float v = freefallPhase(s, curve_p);
            // Set lower bound on freefall value at vmin
            if (v < vmin) {
                return vmin;
            } else {
                return v;
            }
        }
    }
}