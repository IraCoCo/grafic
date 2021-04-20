#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "ext.hpp"
#include "matrix.hpp"


class Model {
public:
    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLsizei indexCount;
};
GLuint createShader(const GLchar* code, GLenum type);
GLuint createProgram(GLuint vsh, GLuint fsh);