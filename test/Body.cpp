#include "Body.h"

bool Body::createShaderProgram() {
    g_shaderProgram = 0;

    const GLchar vsh[] =
        "#version 330\n"
        ""
        "layout(location = 0) in vec3 a_pos;"
        "layout(location = 1) in vec3 a_normal;"
        ""
        "uniform mat4 u_mvp;"
        "uniform mat4 u_mv;"
        "uniform mat3 u_normal;"
        ""
        "out vec3 v_normal;"
        "out vec3 v_pos;"
        ""
        "void main()"
        "{"
        "    vec4 p0 = vec4(a_pos, 1.0);"
        "    v_normal = u_normal * a_normal;"
        "    v_pos = vec3(u_mv * p0);"
        "    gl_Position = u_mvp * p0;"
        "}";

    const GLchar fsh[] =
        "#version 330\n"
        ""
        "in vec3 v_normal;"
        "in vec3 v_pos;"
        ""
        "layout(location = 0) out vec4 o_color;"
        ""
        "void main()"
        "{"
        "   float S = 60.0;"
        "   vec3 color = vec3(0.0, 1.0, 1.0);"
        "   vec3 n = normalize(v_normal);"
        "   vec3 E = vec3(0.0, 0.0, 0.0);"
        "   vec3 L = vec3(5.0, 5.0, 3.0);"
        "   vec3 l = normalize(v_pos - L);"
        "   float d = max(dot(n, -l), 0.3);"
        "   vec3 e = normalize(E - v_pos);"
        "   vec3 h = normalize(-l + e);"
        "   float s = pow(max(dot(n, h), 0.0), S);"
        ""
        "   vec3 _color = color * d + s * vec3(1.0, 1.0, 1.0); "
        "   o_color = vec4(pow(_color, vec3(1.0 / 2.2)), 1.0);"
        "}"
        ;


    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vsh, GL_VERTEX_SHADER);
    fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

    g_shaderProgram = createProgram(vertexShader, fragmentShader);

    g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");
    g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");
    g_uNorm = glGetUniformLocation(g_shaderProgram, "u_normal");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return g_shaderProgram != 0;
}


bool Body::createModel(const vector<Point2D> &points)
{
    const int rotations = 64;
    const int n = points.size();
    const int count_vertices =  n* (rotations) * 6;
    const int count_indices = (n-1) * rotations * 6;
  
    GLfloat* vertices = new GLfloat[count_vertices];
    GLuint* indices = new GLuint[count_indices];

    Point2D v = points[1] - points[0];
    v.normalize();

    vertices[0] = points[0].x;
    vertices[1] = points[0].y;
    vertices[2] = 0.0f;
    vertices[3] = -v.y;
    vertices[4] = v.x;
    vertices[5] = 0.0f;


    v = points[n - 2] - points[n - 1];
    v.normalize();

    vertices[6 * n - 6] = points[n - 1].x;
    vertices[6 * n - 5] = points[n - 1].y;
    vertices[6 * n - 4] = 0.0f;
    vertices[6 * n - 3] = v.y;
    vertices[6 * n - 2] = -v.x;
    vertices[6 * n - 1] = 0.0f;

    for (int i = 6, k = 1; i < (n - 1) * 6; i+=6, k++) 
    {
        Point2D vec1 = points[k] - points[k - 1];
        Point2D vec2 = points[k + 1] - points[k];
        vec1.normalize();
        vec2.normalize();
        Point2D s = vec1 + vec2;
        s.normalize();
        vertices[i] = points[k].x;
        vertices[i+1] = points[k].y;
        vertices[i+2] = 0.0f;
        vertices[i+3] = -s.y;
        vertices[i+4] = s.x;
        vertices[i+5] = 0.0f;
    }


    mat4 model = mat4(1.0f);
    mat4 rotated = rotate(model, radians((GLfloat)360./rotations), vec3(1.0, 0.0, 0.0));
    for (int i = 6 * n; i < count_vertices; i += 6 * n)
    {
        for (int j = 0; j < n * 6; j += 6)
        {
            int id = i + j - 6 * n;
            vec4 prevPos = vec4(vertices[id], vertices[id + 1], vertices[id + 2], 1.0f);
            vec4 prevNormal = vec4(vertices[id + 3], vertices[id + 4], vertices[id + 5], 1.0f);
            
            vec4 resultPos = rotated * prevPos;
            vec4 resultNormal = rotated * prevNormal;
            
            vertices[i + j] = resultPos.x;
            vertices[i + j + 1] = resultPos.y;
            vertices[i + j + 2] = resultPos.z;
            vertices[i + j + 3] = resultNormal.x;
            vertices[i + j + 4] = resultNormal.y;
            vertices[i + j + 5] = resultNormal.z;
        }
    }

    int i, k;
    for (i = 0, k = 0; i < (n - 1) * (rotations - 1) * 6; k++) 
    {
        for (int count = 0; count < n - 1; count++, i += 6, k++) 
        {
            indices[i] = k;
            indices[i + 1] = k + n;
            indices[i + 2] = k + 1;
            indices[i + 3] = k + n;
            indices[i + 4] = k + 1 + n;
            indices[i + 5] = k + 1;
        }
    }

    for (int c = 0, k = n * (rotations - 1); c < n - 1; c++, i += 6)
    {
        indices[i] = c;
        indices[i + 1] = c + k + 1;
        indices[i + 2] = c + k;
        indices[i + 3] = c;
        indices[i + 4] = c + 1;
        indices[i + 5] = c + k + 1;
    }



    glGenVertexArrays(1, &g_model.vao);
    glBindVertexArray(g_model.vao);

    glGenBuffers(1, &g_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, count_vertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);

    g_model.indexCount = count_indices;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    delete[] vertices;
    delete[] indices;
    return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;
}
void Body::draw(GLfloat delta)
{
    // Clear color buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_shaderProgram);
    glBindVertexArray(g_model.vao);

    
    //model = translate(model, vec3(0.0f, 0.0f, 0.0f));
    model = rotate(model, radians(delta * 30.0f),vec3(0.0f, 1.0f, 0.0f));


    mat4 view = lookAt(vec3(10.0f, 0.0f, 30.0f),
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    mat4 MV = view * model;

    glUniformMatrix3fv(g_uNorm, 1, GL_FALSE, value_ptr(transpose(inverse(mat3(MV)))));
    glUniformMatrix4fv(g_uMVP, 1, GL_FALSE, value_ptr(P * MV));
    glUniformMatrix4fv(g_uMV, 1, GL_FALSE, value_ptr(MV));

    glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);

}
void Body::cleanup()
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