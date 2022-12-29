#pragma once

#include <gl/glew.h>

int loadShader(GLenum shader_type, GLuint *shader, const char *source);
int loadProgram(GLuint *program, const char *vertex_shader, const char *geometry_shader, const char *fragment_shader);
