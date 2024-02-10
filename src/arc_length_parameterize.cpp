#include "arc_length_parameterize.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace modelling {

	ArcLengthTable::ArcLengthTable(float deltaS) : m_delta_s(deltaS) { assert(deltaS > 0); }

	void ArcLengthTable::setH(float h) { H = h; }

	void ArcLengthTable::setTopS(float s) { topS = s; }
	
	void ArcLengthTable::addNext(float u) { m_values.push_back(u); }

	void ArcLengthTable::reserve_memory(size_t n) { m_values.reserve(n); }

	float ArcLengthTable::deltaS() const { return m_delta_s; }

	size_t ArcLengthTable::size() const { return m_values.size(); }

	float ArcLengthTable::length() const { return size() * deltaS(); }

	float ArcLengthTable::getH() const { return H; }

	float ArcLengthTable::getTopS() const { return topS; }

	ArcLengthTable::iterator ArcLengthTable::begin() {
		return std::begin(m_values);
	}

	ArcLengthTable::iterator ArcLengthTable::end() { return std::end(m_values); }

	ArcLengthTable::const_iterator ArcLengthTable::begin() const {
		return std::begin(m_values);
	}

	ArcLengthTable::const_iterator ArcLengthTable::end() const {
		return std::end(m_values);
	}

	// Gets the nearest sample U at s
	// Get u value nearest indexed s
	float ArcLengthTable::nearestValueTo(float s) const {
		//if s is in bounds
		int i = indexAt(s);
		return m_values[i];
	}
	//***** ******** ***** *****//

	// Gets the U value (linearly calculated) at s
	// Get exact u value using interpolation from nearest indexed s's
	float ArcLengthTable::operator()(float s) const {
		float start = nearestValueTo(s);
		float s_start = indexAt(s) * m_delta_s;
		
		float end, s_end;
		if (indexAt(s)+1 >= m_values.size()){
			start = nearestValueTo(0);
			s_start = indexAt(0) * m_delta_s;
			end = m_values[1];
			s_end = m_delta_s;
		} else { 
			int i = indexAt(s)+1;
			end = m_values[i];
			s_end = i * m_delta_s;
		}

		float t = (s-s_start)/(s_end-s_start);
		
		return start * (1.f - t) + end * (t);
	}
	//***** ******** ***** *****//

	size_t ArcLengthTable::indexAt(float s) const {
		// Assuming s in [0, length) since private function
		return size_t(s / m_delta_s);
	}

	// Generates the ALP
	// Stores all s values at index
	ArcLengthTable calculateArcLengthTable(HermiteCurve const& curve, float delta_s,
		float delta_u) {
		assert(curve.controlPoints().size() > 0);
		assert(delta_s > 0.f);
		assert(delta_u > 0.f);

		//calc highest point
		float H=0;

		float L_C = curve.arcLength(delta_u);
		int N = floor(L_C/delta_s);	
		ArcLengthTable table(delta_s);
		table.reserve_memory(N);
		table.addNext(0);
		float dS_accum = 0;
		float u = 0;
		int i = 1;
		//Loop here to fill table
		while (u <= 1) {
			if (curve.position(u)[1] > H){
				H = curve.position(u)[1];
				table.setTopS(i*delta_s);
			}
			dS_accum = dS_accum + (glm::length(curve.position(u + delta_u) - curve.position(u)));
			if (dS_accum > delta_s) {
				i++;
				table.addNext(u + delta_u);
				dS_accum = dS_accum - delta_s;
			}
			u = u + delta_u;
		}
		table.setH(H);
		return table;
	}
	//***** ******** ***** *****//

} // namespace modelling