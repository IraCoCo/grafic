#pragma once
#include "Model.h"
#include "tbezier.h"
#include <vector>

using namespace glm;

class Line
{
public:
	Model g_model;
	GLuint g_shaderProgram;
	//GLint g_uMVP;
	//vector<GLfloat> pointXY;
	vector<Point2D> linePoints;
	//vector<GLuint> indices;

	bool createShaderProgram();
	bool createModel();
	void draw();
	void cleanup();
};