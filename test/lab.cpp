#include <iostream>
#include <chrono>
#include "windows.h"
#include "stb_image.h"
#include "Model.h"
#include "Points.h"
#include "Line.h"
#include "Body.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

using namespace std;
using namespace glm;


GLFWwindow* g_window;

chrono::time_point<chrono::system_clock> g_callTime;

Points points;
Line line;
Body body;
bool BodyExist = false;

bool init() {
    // Set initial color of color buffer to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    return points.createShaderProgram() && points.createModel()
        && line.createShaderProgram() && body.createShaderProgram();
}

void reshape(GLFWwindow* window, int width, int height) {
    body.P = perspective(radians(45.0f),float(width) / float(height), 0.1f, 100.f);
    glViewport(0, 0, width, height);
}


bool initOpenGL() {
    // Initialize GLFW functions.
    if (!glfwInit()) {
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
    if (g_window == NULL) {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    // Initialize OpenGL context with.
    glfwMakeContextCurrent(g_window);

    // Set internal GLEW variable to activate OpenGL core profile.
    glewExperimental = true;

    // Initialize GLEW functions.
    if (glewInit() != GLEW_OK) {
        cout << "Failed to initialize GLEW" << endl;
        return false;
    }

    // Ensure we can capture the escape key being pressed.
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set callback for framebuffer resizing event.
    glfwSetFramebufferSizeCallback(g_window, reshape);

    return true;
}

void tearDownOpenGL() {
    // Terminate GLFW.
    glfwTerminate();
}

void calculatePoints() {
    vector<Segment> segment;
    tbezierSO0(points.points, segment);
    line.linePoints.clear();

    for (auto s : segment) {
        for (int i = 0; i < RESOLUTION; ++i) {
            Point2D point = s.calc((double)i / (double)RESOLUTION);
            line.linePoints.push_back(point);
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        int width = 0, height = 0;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        glfwGetWindowSize(window, &width, &height);

        x = x / width * 2. - 1.;
        y = 1. - y / height * 2.;

        
        //points.add(vec2(x, y));
        points.points.push_back(Point2D(x, y));

        if (points.points.size() < 2)
            return;
        if (points.points.size() == 2) {
            line.linePoints = points.points;
            line.createModel();
            return;
        }
        calculatePoints();
        line.createModel();
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (points.points.size() < 2)
            return;
        BodyExist = true;

        body.createModel(line.linePoints);
    }
}



int main() {
    // Initialize OpenGL
    if (!initOpenGL())
        return -1;

    // Initialize graphical resources.
    bool isOk = init();

    if (isOk) {
        //
        //

        g_callTime = chrono::system_clock::now();

        glfwSetMouseButtonCallback(g_window, mouse_button_callback);
        // Main loop until window closed or escape pressed.
        //int i = 0;
       /* if (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(g_window) == 0) {*/
            while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0) {
                // Draw scene.

                auto callTime = chrono::system_clock::now();
                chrono::duration<double> elapsed = callTime - g_callTime;
                g_callTime = callTime;
                //i++;
                points.draw();
                line.draw();
                /*if (i > 360) i = 0;*/
                if (BodyExist)
                    body.draw(elapsed.count());
                // Swap buffers.
                glfwSwapBuffers(g_window);
                // Poll window events.
                glfwPollEvents();
                Sleep(30);
            }
        //}
    }

    // Cleanup graphical resources.
    points.cleanup();
    line.cleanup();
    body.cleanup();

    // Tear down OpenGL.
    tearDownOpenGL();

    return isOk ? 0 : -1;
}


