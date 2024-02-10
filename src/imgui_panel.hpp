#pragma once

#include <iosfwd>
#include <string>

#include "givio.h"
#include "givr.h"
#include "imgui/imgui.h"

namespace imgui_panel {

extern bool showPanel;
extern ImVec4 clear_color;

// playback speed
extern bool updateSpeed;
extern int speed;

// look ahead
extern bool updateH;
extern int value;

// loading
extern bool rereadControlPoints;
extern bool clearControlPointsFilePath;
extern std::string controlPointsFilePath;

// animation
extern bool play;
extern bool resetView;
extern bool resetSimulation;

// lambda function
extern std::function<void(void)> draw;

} // namespace panel
