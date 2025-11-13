// The Geometry Factory by Daniel Motilla Monreal
// When I started this, I was stupid; now I'm a bit less stupid
// Huge thanks to Terry A. Davis for the inspiration to be a better programmer

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// Global state
static GLFWwindow* window = NULL;
static GLuint fill_program = 0;
static GLuint line_program = 0;
static GLuint vao = 0;
static GLuint vbo = 0;
static float* vertices = NULL;
static size_t vertex_count = 0;
static bool initialized = false;

// Basic vertex shader that passes through 3D positions
static const char* vertex_shader_src =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "void main() { gl_Position = vec4(aPos, 1.0); }\n";

// Fragment shader for grey fill
static const char* fill_fragment_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() { FragColor = vec4(0.7, 0.7, 0.7, 1.0); }\n";

// Fragment shader for black wireframe
static const char* line_fragment_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() { FragColor = vec4(0.0, 0.0, 0.0, 1.0); }\n";

// Utility function to compile a shader from source
static GLuint compile_shader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation error: %s\n", info_log);
        return 0;
    }
    return shader;
}

// Initialize OpenGL context and window
EXPORT bool init_geometry_factory(int width, int height) {
    if (initialized) return true;
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    
    // Try to create window
    window = glfwCreateWindow(width, height, "The Geometry Factory", NULL, NULL);
    if (!window) {
        const char* description;
        int code = glfwGetError(&description);
        fprintf(stderr, "Failed to create GLFW window: %s (code: %d)\n", description, code);
        glfwTerminate();
        return false;
    }
    
    // Make OpenGL context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    
    // Set up OpenGL state
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Fill mode for solid color
    glEnable(GL_LINE_SMOOTH);  // Smooth lines for wireframe
    glLineWidth(1.0f);  // Thin lines for wireframe
    
    // Print OpenGL info
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    fprintf(stderr, "OpenGL Info:\n");
    fprintf(stderr, "  Vendor: %s\n", vendor);
    fprintf(stderr, "  Renderer: %s\n", renderer);
    fprintf(stderr, "  Version: %s\n", version);
    
    // Create fill shader program
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fill_fragment = compile_shader(GL_FRAGMENT_SHADER, fill_fragment_src);
    
    fill_program = glCreateProgram();
    glAttachShader(fill_program, vertex_shader);
    glAttachShader(fill_program, fill_fragment);
    glLinkProgram(fill_program);
    
    glDeleteShader(fill_fragment);
    
    // Create line shader program
    GLuint line_fragment = compile_shader(GL_FRAGMENT_SHADER, line_fragment_src);
    
    line_program = glCreateProgram();
    glAttachShader(line_program, vertex_shader);
    glAttachShader(line_program, line_fragment);
    glLinkProgram(line_program);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(line_fragment);
    
    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    initialized = true;
    return true;
}

// Update triangle fan vertices
EXPORT bool update_triangle_fan(float* new_vertices, size_t count) {
    if (!initialized) return false;
    
    // Free old vertices if they exist
    if (vertices) {
        free(vertices);
    }
    
    // Allocate and copy new vertices
    vertices = (float*)malloc(count * 3 * sizeof(float));
    if (!vertices) return false;
    
    memcpy(vertices, new_vertices, count * 3 * sizeof(float));
    vertex_count = count;
    
    // Update GPU buffer
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    return true;
}

// Render one frame
EXPORT bool render_frame() {
    if (!initialized || !window) return false;
    
    if (glfwWindowShouldClose(window)) {
        return false;
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vao);
    
    // Draw filled triangles
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(fill_program);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
    
    // Draw wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glUseProgram(line_program);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    return true;
}

// Cleanup resources
EXPORT void cleanup_geometry_factory() {
    if (vertices) {
        free(vertices);
        vertices = NULL;
    }
    
    if (initialized) {
        // Signal the window to close
        if (window) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            // Process events to ensure the close signal is handled
            glfwPollEvents();
        }

        // Delete OpenGL resources
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(fill_program);
        glDeleteProgram(line_program);
        
        // Destroy window and terminate GLFW
        if (window) {
            glfwDestroyWindow(window);
            window = NULL;
        }
        glfwTerminate();
        
        initialized = false;
    }
}
