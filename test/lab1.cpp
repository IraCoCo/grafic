#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

using namespace std;

GLFWwindow* g_window;

GLuint g_shaderProgram;
GLint g_uMVP;
GLint g_uMV;

class Model
{
public:
    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLsizei indexCount;
};

Model g_model;

GLuint createShader(const GLchar* code, GLenum type)
{
    GLuint result = glCreateShader(type);

    glShaderSource(result, 1, &code, NULL);
    glCompileShader(result);

    GLint compiled;
    glGetShaderiv(result, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(result, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0)
        {
            char* infoLog = (char*)alloca(infoLen);
            glGetShaderInfoLog(result, infoLen, NULL, infoLog);
            cout << "Shader compilation error" << endl << infoLog << endl;
        }
        glDeleteShader(result);
        return 0;
    }

    return result;
}

GLuint createProgram(GLuint vsh, GLuint fsh)
{
    GLuint result = glCreateProgram();

    glAttachShader(result, vsh);
    glAttachShader(result, fsh);

    glLinkProgram(result);

    GLint linked;
    glGetProgramiv(result, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(result, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0)
        {
            char* infoLog = (char*)alloca(infoLen);
            glGetProgramInfoLog(result, infoLen, NULL, infoLog);
            cout << "Shader program linking error" << endl << infoLog << endl;
        }
        glDeleteProgram(result);
        return 0;
    }

    return result;
}

bool createShaderProgram()
{
    g_shaderProgram = 0;

    
    const GLchar vsh[] =
        "#version 330\n"
        ""
        "layout(location = 0) in vec2 a_pos;"
        "uniform mat4 u_mvp;"
        "uniform mat4 u_mv;"
        ""
        "out vec3 v_normal;"
        "out vec3 v_pos;"
        ""
        "float f(vec2 p) {return 2 * (1 - p.x * p.y) * sin(1 - p.x * p.y);}"
        "vec3 grad(vec2 p) {return vec3(2 * p.y * sin(1 - p.x * p.y) + 2 * p.y * (1 - p.x * p.y) * cos(1 - p.x * p.y), 1.0, 2 * p.x * sin(1 - p.x * p.y) + 2 * p.x * (1 - p.x * p.y) * cos(1 - p.x * p.y));}"
        ""
        "void main()"
        "{"
        "    float y = f(a_pos);"
        "    vec4 p0 = vec4(a_pos[0], y, a_pos[1], 1.0);"
        "    v_normal = transpose(inverse(mat3(u_mv))) * normalize(grad(a_pos));"
        "    v_pos = vec3(u_mv * p0);"
        "    gl_Position = u_mvp * p0;"
        "}"
        ;
    
    
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
        "   float S = 20;"
        "   vec3 color = vec3(1, 0, 0);"
        "   vec3 n = normalize(v_normal);"
        "   vec3 E = vec3(0, 0, 0);"
        "   vec3 L = vec3(5, 5, 0);"
        "   vec3 I = normalize(v_pos - L);"
        "   float d = max(dot(n, -I), 0.1);"
        "   vec3 e = normalize(E - v_pos);"
        "   vec3 h = normalize(-I + e);"
        "   float s = pow(max(dot(n, h), 0), S);"
        "   o_color = vec4(color * d + s * vec3(1.0, 1.0, 1.0), 1.0);"
        "}"
        ;



    GLuint vertexShader, fragmentShader;

    vertexShader = createShader(vsh, GL_VERTEX_SHADER);
    fragmentShader = createShader(fsh, GL_FRAGMENT_SHADER);

    g_shaderProgram = createProgram(vertexShader, fragmentShader);
    //Матрицы
    g_uMV = glGetUniformLocation(g_shaderProgram, "u_mv");
    g_uMVP = glGetUniformLocation(g_shaderProgram, "u_mvp");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return g_shaderProgram != 0;
}
bool createModel()
{
    const int n = 100;
    float* vertices = new float[n * n * 2];
    int* indices = new int[(n - 1) * (n - 1) * 6];

    

    glGenVertexArrays(1, &g_model.vao);
    glBindVertexArray(g_model.vao);

    glGenBuffers(1, &g_model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_model.vbo);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_model.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_model.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
    g_model.indexCount = 6;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid*)(2 * sizeof(GLfloat)));

    return g_model.vbo != 0 && g_model.ibo != 0 && g_model.vao != 0;

    delete[] vertices;
    delete[] indices;
}



bool init()
{
    // Set initial color of color buffer to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    return createShaderProgram() && createModel();
}

void reshape(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void draw()
{
    // Clear color buffer.
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shaderProgram);
    glBindVertexArray(g_model.vao);

    glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, NULL);
}

void cleanup()
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

bool initOpenGL()
{
    // Initialize GLFW functions.
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return false;
    }

    // Request OpenGL 3.3 without obsoleted functions.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window.
    g_window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
    if (g_window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    // Initialize OpenGL context with.
    glfwMakeContextCurrent(g_window);

    // Set internal GLEW variable to activate OpenGL core profile.
    glewExperimental = true;

    // Initialize GLEW functions.
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW" << endl;
        return false;
    }

    // Ensure we can capture the escape key being pressed.
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set callback for framebuffer resizing event.
    glfwSetFramebufferSizeCallback(g_window, reshape);
    return true;
}


void tearDownOpenGL()
{
    // Terminate GLFW.
    glfwTerminate();
}

int main()
{
    // Initialize OpenGL
    if (!initOpenGL())
        return -1;

    // Initialize graphical resources.
    bool isOk = init();

    if (isOk)
    {
        // Main loop until window closed or escape pressed.
        while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0)
        {
            // Draw scene.
            draw();

            // Swap buffers.
            glfwSwapBuffers(g_window);
            // Poll window events.
            glfwPollEvents();
        }
    }

    // Cleanup graphical resources.
    cleanup();

    // Tear down OpenGL.
    tearDownOpenGL();
    return isOk ? 0 : -1;
}
