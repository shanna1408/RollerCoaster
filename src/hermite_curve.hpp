#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "givr.h"

namespace modelling {
	class HermiteCurve {
	public:
		struct ControlPoint {
			glm::vec3 position;
			glm::vec3 tangent;
		};
		using ControlPoints = std::vector<ControlPoint>;
		static HermiteCurve::ControlPoints
			buildControlPoints(std::vector<glm::vec3> posistions);
		static void calculateCatmullRomTangents(HermiteCurve::ControlPoints& cps);
		static void calculateCanonicalTangents(HermiteCurve::ControlPoints& cps, float c);

		HermiteCurve() = default;
		explicit HermiteCurve(ControlPoints controlPoints);

		glm::vec3 operator()(float U) const;
		glm::vec3 position(float U) const;

		ControlPoints const& controlPoints() const;
		ControlPoints& controlPoints();

		float arcLength(float dU) const;
		std::vector<glm::vec3> sample(size_t number_of_samples) const;

		//Render tracks
		givr::geometry::MultiLine controlPointGeometry() const;
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
			controlPointFrameGeometry() const;
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
			sampledGeometry(size_t number_of_samples) const;

	private:
		ControlPoints m_cps;

		std::pair<float, size_t> localize(float U) const;

		//***** STUDENTS TO-DO *****//
		// Gets the position at U (global parameter)
		glm::vec3 positionInSegment(std::pair<float, size_t> u_seg) const;
		//***** ******** ***** *****//
	};

} // namespace modelling