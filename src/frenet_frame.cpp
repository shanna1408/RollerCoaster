#include "frenet_frame.hpp"
#include <iostream>
#include "givr.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace modelling {
    FrenetFrame::FrenetFrame(ArcLengthTable arc_table) : table(arc_table) { }

    glm::vec3 FrenetFrame::get_normal(glm::vec3 t0, glm::vec3 t1, glm::vec3 tangent) const {
        glm::vec3 n = glm::normalize(t1-t0);
        glm::vec3 n_orth = (n-(glm::dot(n, tangent))*tangent);
        n_orth = glm::normalize(n_orth);
        return n_orth;
    }
    
    glm::vec3 FrenetFrame::get_aalp(glm::vec3 t0, glm::vec3 t1, float c1, glm::vec3 n_orth) const{
        return ((2*glm::length(glm::cross(t1, t0)))/c1)*n_orth;
    }

    mat4f FrenetFrame::getM(HermiteCurve curve, ArcLengthTable table, float s, float v, int h) const{
        // float h = 5;
        float h1, h2;
        if (s < h){
            h1 = table.length()-(h-s);
        } else{
            h1 = s-h;
        }
        if (s >= (table.length()-h)){
            h2 = (s+h)-table.length();
        } else{
            h2 = s+h;
        }

        glm::vec3 a = curve(table(s)) - curve(table(h1));
        glm::vec3 b = curve(table(h2)) - curve(table(s));
        glm::vec3 c = curve(table(h2)) - curve(table(h1));

        glm::vec3 t0 = glm::normalize(a);
        glm::vec3 t1 = glm::normalize(b);
        glm::vec3 T = glm::normalize(c);

        glm::vec3 a_alp = get_aalp(t0, t1, glm::length(c), get_normal(t0, t1, T));
        glm::vec3 a_orth = (v*v)*a_alp;
        glm::vec3 g_orth = (g-(glm::dot(g, T))*T);
        glm::vec3 N = a_orth-g_orth;
        N = N/glm::length(N);
        glm::vec3 B = glm::cross(N, T);
        B = B/glm::length(B);

        mat4f M;
        glm::vec3 P0 = curve(table(s));
        M[0] = glm::vec4(B, 0.f);
        M[1] = glm::vec4(N, 0.f);
        M[2] = glm::vec4(T, 0.f);
        M[3] = glm::vec4(P0, 1.f);
        return M;
    }

    void FrenetFrame::renderTrack(HermiteCurve& curve, givr::InstancedRenderContext<givr::geometry::Mesh, givr::style::PhongStyle>& track_renders, int h){
        float s = 0;
        float ds = table.length()/1000.f;
        float u;
        while (s < table.length()-ds){
            u = table(s);
		    auto curve_p = curve(u);
            auto M = getM(curve, table, s, 1, h);
            addInstance(track_renders, M);
            s+= ds;
        }
    }
}