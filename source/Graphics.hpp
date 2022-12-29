#pragma once

#include <glm/glm.hpp>

namespace Graphics {
	bool init();
	void deinit();
	void render();
	void resize(glm::ivec2 dims);
}
