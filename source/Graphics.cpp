#include "Graphics.hpp"

#include "Logger.hpp"
#include "GLTools.hpp"

const glm::vec3 QUAD_DATA[4] = {
	{ -0.5f, -0.5f, 0.0f },
	{ -0.5f,  0.5f, 0.0f },
	{  0.5f, -0.5f, 0.0f },
	{  0.5f,  0.5f, 0.0f }
};
const char *const SHADER_VERT = R"(
#version 330 core

layout(location = 0) in vec3 position_in;

void main() {
	gl_Position = vec4(position_in, 1.0f);
}
)";
const char *const SHADER_FRAG = R"(
#version 330 core

out vec4 colour_out;

void main() {
	colour_out = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

GLuint program = 0, vao = 0, vbo = 0;

bool Graphics::init() {
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		logger("Failed to initialize GLEW.");
		return false;
	}

	enable_gl_debug_output();

	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	//Loading shaders
	int ret = load_program(program, SHADER_VERT, nullptr, SHADER_FRAG);
	if (ret) {
		logger("Failed to load shaders.");
		return false;
	}

	//Generate tris buffer and vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(0);

	glUseProgram(program);

	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_DATA), QUAD_DATA, GL_STATIC_DRAW);

	logger("Successfully initialised graphics.");
	return true;
}

void Graphics::deinit() {
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	logger("Successfully deinitialised graphics.");
}

void Graphics::render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Graphics::resize(glm::ivec2 dims) {
	glViewport(0, 0, dims.x, dims.y);
}
