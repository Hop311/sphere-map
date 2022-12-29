
#include "Window.hpp"
#include "Logger.hpp"

int main() {
	if (!Window::init(1280, 720, "sphere-map")) {
		logger("Window initialisation failed.");
		return -1;
	}
	Window::run();
	Window::deinit();
	return 0;
}
