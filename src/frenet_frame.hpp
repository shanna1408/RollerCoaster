#pragma once

#include "hermite_curve.hpp"
#include "arc_length_parameterize.hpp"
#include <string>
#include "givr.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace givr;
using namespace givr::geometry;
using namespace givr::style;

namespace modelling {
	class FrenetFrame {
        public:
            FrenetFrame() = default;
            explicit FrenetFrame(ArcLengthTable arc_table);

            mat4f getM(HermiteCurve curve, ArcLengthTable table, float s, float v, int h) const;
            void renderTrack(HermiteCurve& curve, givr::InstancedRenderContext<givr::geometry::Mesh, givr::style::PhongStyle>& track_renders, int h);

        private:
            glm::vec3 g = {0,-9.81,0};
            ArcLengthTable table;

            glm::vec3 get_aalp(glm::vec3 t0, glm::vec3 t1, float c1, glm::vec3 n_orth) const;
            glm::vec3 get_normal(glm::vec3 t0, glm::vec3 t1, glm::vec3 tangent) const;  
    };
}