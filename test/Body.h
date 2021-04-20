#pragma once
#include "Model.h"
#include "tbezier.h"
#include <vector>

using namespace glm;

class Body
{
public:
	Model g_model;
	GLuint g_shaderProgram;
	GLint g_uMVP, g_uMV, g_uNorm;
	mat4 P = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	mat4 model = scale(mat4(1.0f), vec3(10.0f));
	vector<Point2D> normals;

	bool createShaderProgram();
	bool createModel(const vector<Point2D> &points);
	void draw(GLfloat delta);
	void cleanup();
};
