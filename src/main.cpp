#include "givio.h"
#include "givr.h"
#include "picking_controls.h"
#include "turntable_controls.h"

#include "imgui_panel.hpp"
#include "arc_length_parameterize.hpp"
#include "curve_file_io.hpp"
#include "hermite_curve.hpp"
#include "speed_profile.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace giv;
using namespace giv::io;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;


// This boilerplate sets up work for the Hermite Curve and Arc Length Table. Each will indicate which functions to complete
// You are also expected to implement the Single Particle kinamatics and Framing of the curve (and bonuses) from scratch
// THis main file contains the program loop of the simulation with controls define in the key bindings here and the imgui_panel files
// Once on the single particle kinamatics section, you may hard code values (chain speed and segment starts) and assign the desired
// instead of the default. Lastly, there are 3 coasters in the obj files. We also gave 3 Arc Length parameterized versions for students who
// need to debug or cannot finish the ALP tables (you will not get full marks). Coaster 2 is small compared to the model of the cart so apply 
// scaling to the models when needed. You may also use your own coaster


const std::vector<glm::vec3> default_cps = {
	{0.f, 0.f, 10.f},
	{10.f, 0.f, 0.f},
	{0.f, 0.f, -10.f},
	{-10.f, 0.f, 0.f}
};

// program entry point
int main(void) {
	// initialize OpenGL and window
	GLFWContext glContext;
	glContext.glMajorVesion(3)
		.glMinorVesion(3)
		.glForwardComaptability(true)
		.glCoreProfile()
		.glAntiAliasingSamples(4)
		.matchPrimaryMonitorVideoMode();
	std::cout << glfwVersionString() << '\n';

	// setup window (OpenGL context)
	ImGuiWindow window = glContext.makeImGuiWindow(Properties()
		.size(dimensions{ 800, 600 })
		.title("Assignment 1: Roller Coaster")
		.glslVersionString("#version 330 core"));
	// set our imgui update function
	panel::update_lambda_function = imgui_panel::draw;

	// create viewing camera and controls
	ViewContext view = View(TurnTable(), Perspective());
	TurnTableControls controls(window, view.camera);

	// create key bingdings
	auto reset_camera = [&](auto event) { view.camera.reset(); };
	auto toggle_panel = [&](auto event) {
		if (event.action == GLFW_PRESS)
			imgui_panel::showPanel = !imgui_panel::showPanel;
	};
	window.keyboardCommands() 
		| Key(GLFW_KEY_V, reset_camera) 
		| Key(GLFW_KEY_P, toggle_panel);


	// initial curve --
	modelling::HermiteCurve curve(
		modelling::HermiteCurve::buildControlPoints(default_cps)
	);

	//To load the arc length parameterized curve (only worth part marks so only do this if you need to):
	// curve = modelling::readHermiteCurveFromFile("../models/roller_coaster_1_ALP.txt").value();
	curve = modelling::readHermiteCurveFrom_OBJ_File("../models/roller_coaster_2.obj").value();

	// ALP table
	float du = (1.f/10000.f)*(1.f/20.f);
	float ds = curve.arcLength(du) / 1000;
	modelling::ArcLengthTable table = calculateArcLengthTable(curve, ds, du);

	// Initialize speed profile
	modelling::SpeedProfile speed(table.getH(), table.getTopS(), table.length());
	std::cout << "Highest H:" << speed.getH() << std::endl;

	// Control points frame Geometry
	PolyLine cp_geometry = curve.controlPointFrameGeometry();
	GL_Line cp_style = GL_Line(Width(15.), Colour(0.5, 1.0, 0.0));
	RenderContext cp_render = createRenderable(cp_geometry, cp_style);

	// Control points Geometry
	MultiLine cp_t_geometry = curve.controlPointGeometry();
	GL_Line cp_t_style = GL_Line(Width(25.), Colour(1.0, 0.0, 0.3));
	RenderContext cp_t_render = createRenderable(cp_t_geometry, cp_t_style);

	// geometry for curve
	PolyLine track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);
	GL_Line track_style = GL_Line(Width(15.), Colour(0.2, 0.7, 1.0));
	RenderContext track_render = createRenderable(track_geometry, track_style);

	// Cart
	Mesh cart_geometry = Mesh(Filename("../models/monkey.obj"));
	PhongStyle cart_style = Phong(Colour(1.f, 1.f, 0.f), LightPosition(100.f, 100.f, 100.f));
	InstancedRenderContext cart_renders = createInstancedRenderable(cart_geometry, cart_style);
	
	size_t cp_index = 0;
	float s = 0.f;
	float v = 0.f;

	// main loop
	// To load a model place it in the "models" directory, build, then type "./models/[name].obj" and press load.
	// If the above does not work, manually copy the model into the models folder in the output directory, then try and reload the track
	// Backspace isnt enabled when the panel is over the window, please move the panel off the window to backspace.
	//
	mainloop(std::move(window), [&](float dt /**** Time since last frame ****/) {
		if (imgui_panel::resetView)
			view.camera.reset();

		if (imgui_panel::rereadControlPoints) {
			// load points (if possible)
			std::optional<modelling::HermiteCurve> optional_curve 
			= modelling::readHermiteCurveFrom_OBJ_File(
				imgui_panel::controlPointsFilePath

			);
			if (optional_curve) {
				curve = optional_curve.value();

				cp_geometry = curve.controlPointFrameGeometry();
				cp_t_geometry = curve.controlPointGeometry();
				track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);

				updateRenderable(cp_geometry, cp_style, cp_render);
				updateRenderable(cp_t_geometry, cp_t_style, cp_t_render);
				updateRenderable(track_geometry, track_style, track_render);

				cp_index = 0;
			}
		}

		if (imgui_panel::resample) {
			track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);
			updateRenderable(track_geometry, track_style, track_render);
		}

		// Simulation
		if (imgui_panel::play) {
			// increment and wrap
			cp_index = (cp_index + 1) % curve.controlPoints().size();
			s = std::fmod((s+(v*dt)), table.length());
		}

		// Render cart at position u
		float u = table(s);
		auto curve_p = curve(u);
		auto M = scale(translate(mat4f{ 1.f }, curve_p), vec3f{ 0.75 });
		addInstance(cart_renders, M);

		v = speed.getSpeed(s, curve_p, v);
		std::cout << "v:" << v << '\n' << std::endl;

		// render
		auto color = imgui_panel::clear_color;
		glClearColor(color.x, color.y, color.z, color.z);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view.projection.updateAspectRatio(window.width(), window.height());

		draw(cp_render, view);
		draw(cp_t_render, view);
		draw(track_render, view);
		draw(cart_renders, view);
	});
	return EXIT_SUCCESS;
}