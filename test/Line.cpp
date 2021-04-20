#include "Line.h"

bool Line::createModel()
{
	size_t vertSize = linePoints.size() * 2;
	size_t idxSize = linePoints.size();

	GLfloat* vertices = new GLfloat[vertSize];
	GLuint* indices = new GLuint[idxSize];

	int i = 0;
	for (Point2D p : linePoints) {
		vertices[i] = p.x;
		vertices[i + 1] = p.y;
		indices[i / 2] = i / 2;
		i = i + 2;
	}



	glGenVertexArrays(1, &g_model.vao);
	glBindVertexArray(g_model.vao);

	glGenBuffers(1, &g_model.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize * sizeof(GLuint), indices, GL_STATIC_DRAW);
	
	g_model.indexCount = idxSize;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid*)0);
	
	delete[] vertices;
	delete[] indices;

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}

bool Line::createShaderProgram()
{
	const GLchar vsh[] =
		"#version 330\n"
		""
		"layout(location = 0) in vec2 a_position;"
		""
		""
		"void main()"
		"{"
		"    gl_Position = vec4(a_position, 0.0, 1.0);"
		"}"
		;
	const GLchar fsh[] =
		"#version 330\n"
		""
		"layout(location = 0) out vec4 o_color;"
		""
		"void main()"
		"{"
		"   o_color = vec4(0.0);"
		"}"
		;

	GLuint vertexShader, fragmentShader;

	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	g_shaderProgram = createProgram(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return g_shaderProgram != 0;
}

void Line::draw()
{
	glUseProgram(g_shaderProgram);
	glBindVertexArray(g_model.vao);
	glDrawElements(GL_LINE_STRIP, g_model.indexCount, GL_UNSIGNED_INT, NULL);

	/*mat4 MVP = P * lookAt;
	glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, value_ptr(MVP));
	glDrawElements(GL_LINE_STRIP, g_model.indexCount, GL_UNSIGNED_INT, NULL);*/
}

void Line::cleanup()
{
	if (g_shaderProgram != 0)
		glDeleteProgram(g_shaderProgram);
	if (g_model.vbo != 0)
		glDeleteBuffers(1, &g_model.vbo);
	if (g_model.ibo != 0)
		glDeleteBuffers(1, &g_model.ibo);
	if (g_model.vao != 0)
		glDeleteVertexArrays(1, &g_model.vao);
}