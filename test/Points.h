#pragma once
#include "Model.h"
#include "tbezier.h"
#include <vector>

using namespace glm;
using namespace std;

class Points
{
public:
	Model g_model;
	GLuint g_shaderProgram;
	GLint g_uMV;

	vector<Point2D> points;


	const float sideLength = 5.0f;
	int countPoints = 0;
	int curIndex = 0;

	bool createShaderProgram();
	bool createModel();
	void draw();
	void cleanup();
};