#include "Window.hpp"

#include "Logger.hpp"
#include "Graphics.hpp"

#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include <thread>
#include <mutex>
#include <vector>

static volatile bool loop_run_flag = false;
static std::mutex input_mutex;

struct key_event { int key, scancode, action, mods; };
static std::vector<key_event> key_events;

static struct {
	GLFWwindow *glfw_ptr;
	bool resized;
	glm::ivec2 dims;
} window;

static void error_callback(int err, const char *desc) {
	logger("GLFW error ", err, ": ", desc);
}
static void framebuffer_size_callback(GLFWwindow *window_ptr, int width, int height) {
	std::lock_guard<std::mutex> guard{ input_mutex };
	if (window_ptr != window.glfw_ptr)
		logger("Unknown window ", window_ptr, " calling framebuffer_size_callback (main window is ", window.glfw_ptr, ").");
	window.dims = { width, height };
	window.resized = true;
}
static void key_callback(GLFWwindow *window_ptr, int key, int scancode, int action, int mods) {
	std::lock_guard<std::mutex> guard{ input_mutex };
	if (window_ptr != window.glfw_ptr)
		logger("Unknown window ", window_ptr, " calling key_callback (main window is ", window.glfw_ptr, ").");
	key_events.push_back({ key, scancode, action, mods });
}

bool Window::init(int width, int height, const char *title) {
	if (window.glfw_ptr) {
		logger("Window has already been initialised!");
		return false;
	}
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		logger("Failed to initialise GLFW.");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window.glfw_ptr = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!window.glfw_ptr) {
		logger("Failed to open GLFW window.");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window.glfw_ptr);
	glfwSetFramebufferSizeCallback(window.glfw_ptr, framebuffer_size_callback);
	glfwSetKeyCallback(window.glfw_ptr, key_callback);

	logger("Successfully initialised GLFW.");

	if (!Graphics::init()) {
		logger("Failed to initialize graphics.");
		return false;
	}

	window.dims = { width, height };
	window.resized = true;

	return true;
}

void Window::deinit() {
	if (!window.glfw_ptr) {
		logger("Window has not been initialised!");
		return;
	}
	Graphics::deinit();
	glfwDestroyWindow(window.glfw_ptr);
	window.glfw_ptr = nullptr;
	glfwTerminate();
	logger("Successfully deinitialised GLFW.");
}

const uint64_t TARGET_TPS = 60;
const double TARGET_SPT = 1.0 / double(TARGET_TPS);

void loop_function() {
	logger("Started window loop.");
	glfwMakeContextCurrent(window.glfw_ptr);

	double last_second = glfwGetTime(), last_loop = last_second, tick_time_passed = 0.0;
	uint64_t frame_count = 0, tick_count = 0, fps_display = 0, tps_display = 0;

	while (loop_run_flag) {
		const double current_time = glfwGetTime();
		tick_time_passed += current_time - last_loop;

		if (tick_time_passed >= TARGET_SPT) {
			// Tick
			do {
				tick_count++;
				tick_time_passed -= TARGET_SPT;

				{
					std::lock_guard<std::mutex> guard{ input_mutex };
					if (window.resized) {
						Graphics::resize(window.dims);
						window.resized = false;
					}
					for (auto e : key_events)
						if (e.key  == GLFW_KEY_ESCAPE && e.action == GLFW_PRESS) Window::close();
				}
			} while (tick_time_passed >= TARGET_SPT);

			// Frame
			frame_count++;

			Graphics::render();

			glfwSwapBuffers(window.glfw_ptr);
		}

		// Trigger each second
		if (current_time - last_second >= 1.0) {
			last_second = current_time;
			fps_display = frame_count;
			tps_display = tick_count;
			frame_count = 0;
			tick_count = 0;
			if (fps_display != TARGET_TPS || tps_display != TARGET_TPS)
				logger("FPS: ", fps_display, ", TPS: ", tps_display);
		}
		last_loop = current_time;
	}

	glfwMakeContextCurrent(nullptr);
	logger("Finishing window loop.");
}

void Window::run() {
	if (!window.glfw_ptr) {
		logger("Window has not been initialised!");
		return;
	}
	glfwMakeContextCurrent(nullptr);

	logger("Starting window loop.");
	loop_run_flag = true;
	std::thread loop_thread{ &loop_function };

	while (!glfwWindowShouldClose(window.glfw_ptr))
		glfwWaitEvents();

	logger("Telling window loop to finish.");
	loop_run_flag = false;
	loop_thread.join();

	logger("Finished window loop.");
	glfwMakeContextCurrent(window.glfw_ptr);
}

void Window::close() {
	if (window.glfw_ptr) glfwSetWindowShouldClose(window.glfw_ptr, GL_TRUE);
	else logger("window == nullptr");
}
