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

    void SpeedProfile::setDecel(bool val){
        decel = val;
    }

    float SpeedProfile::liftingPhase() const { 
        return vmin; }

    float SpeedProfile::freefallPhase(float s, glm::vec3 curve_p) const { 
        return sqrt(2.f*g*(H-curve_p[1]));
    }

    float SpeedProfile::deceleration(float s, float vdec) const {
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
            return std::max(0.05f, deceleration(s, vdec));
        }

        decel = false;
        // If current position is before highest point of curve, use constant v
        if (s <= topS) {
            return liftingPhase();
        // Otherwise, enter freefall
        } else {
            // Set lower bound on freefall value at vmin
            return std::max(freefallPhase(s, curve_p), vmin);
        }
    }
}