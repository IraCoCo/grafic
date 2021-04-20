#include "Points.h"


bool Points::createModel()
{
	const GLfloat vertices[] =
	{
		//  x    y    
		-0.3f, -0.5f, 
		 0.5f, -0.5f, 
		 0.5f,  0.5f, 
		-0.3f,  0.5f, 
	};

	const GLuint indices[] =
	{
		0, 1, 2, 2, 3, 0
	};
	glGenVertexArrays(1, &g_model.vao);
	glBindVertexArray(g_model.vao);

	glGenBuffers(1, &g_model.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &g_model.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	g_model.indexCount = 6;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid*)0);

	return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0; 


}

bool Points::createShaderProgram()
{
	const GLchar vsh[] =
		"#version 330\n"
		""
		"layout(location = 0) in vec2 a_position;"
		""
		"uniform mat4 u_mv;"
		""
		"void main()"
		"{"
		"    gl_Position = u_mv * vec4(a_position, 0.0, 1.0);"
		"}"
		;
	const GLchar fsh[] =
		"#version 330\n"
		""
		""
		"layout(location = 0) out vec4 o_color;"
		""
		"void main()"
		"{"
		"   o_color = vec4(1.0, 0.0, 0.0, 1.0);"
		"}"
		;

	GLuint vertexShader, fragmentShader;

	vertexShader = createShader(vsh, GL_VERTEX_SHADER);
	fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

	g_shaderProgram = createProgram(vertexShader, fragmentShader);
	g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return g_shaderProgram != 0;
}

void Points::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(g_shaderProgram);
	glBindVertexArray(g_model.vao);

	for (Point2D p : points) {
		auto model = mat4(1.0f);

		model = translate(model, vec3(p.x, p.y, 0.0f));
		model = scale(model, vec3(0.04f));
		glUniformMatrix4fv(g_uMV, 1, GL_FALSE, value_ptr(model));
		glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, 0);
	}
	/*mat4 MVP = P * lookAt;
	glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, value_ptr(MVP));
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);*/
}

void Points::cleanup()
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