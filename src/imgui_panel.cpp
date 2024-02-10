#include "imgui_panel.hpp"

#include <array>

namespace imgui_panel {
	// default values
	bool showPanel = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool updateH = true;
	int value = 5;

	bool updateSpeed = true;
	int speed = 1;

	// loading
	bool rereadControlPoints = false;
	bool clearControlPointsFilePath = false;
	std::string controlPointsFilePath = "./roller_coaster.obj";

	// animation
	bool play = false;
	bool resetView = false;
	bool resetSimulation = false;

	std::function<void(void)> draw = [](void) {
		if (showPanel && ImGui::Begin("panel", &showPanel, ImGuiWindowFlags_MenuBar)) {
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Close", "(P)"))
						showPanel = false;
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Background Color"))
				ImGui::ColorEdit3("Clear color", (float*)&clear_color);

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Playback Speed")) {
				ImGui::SliderInt("Speed", &speed, 1, 10);
			}

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Look Ahead")) {
				ImGui::SliderInt("Value", &value, 1, 10);
				updateH = ImGui::Button("Update look ahead");
			}

			// ImGui::Spacing();
			// if (ImGui::CollapsingHeader("Loading control points")) {
			// 	static std::array<char, 64> buffer;
			// 	ImGui::InputText("(OBJ) file", buffer.data(), buffer.size());

			// 	rereadControlPoints = ImGui::Button("Load");
			// 	ImGui::SameLine();
			// 	clearControlPointsFilePath = ImGui::Button("Clear");

			// 	if (rereadControlPoints) controlPointsFilePath = buffer.data();
			// 	if (clearControlPointsFilePath) {
			// 		buffer = std::array<char, 64>();
			// 		clearControlPointsFilePath = false;
			// 	}
			// }

			ImGui::Spacing();
			ImGui::Separator();
			if (ImGui::Button(play ? "Pause" : "Play"))
				play = !play;

			ImGui::Spacing();
			ImGui::Separator();
			resetView = ImGui::Button("Reset view");

			ImGui::Spacing();
			ImGui::Separator();
			resetSimulation = ImGui::Button("Reset simulation");

			ImGui::Spacing();
			ImGui::Separator();
			float frame_rate = ImGui::GetIO().Framerate;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / frame_rate, frame_rate);

			ImGui::End();
		}
	};
} // namespace panel