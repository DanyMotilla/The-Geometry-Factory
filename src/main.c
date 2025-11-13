// The Geometry Factory by Daniel Motilla Monreal
// When I started this, I was stupid; now I'm a bit less stupid
// Huge thanks to Terry A. Davis for the inspiration to be a better programmer

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// Basic vertex shader that passes through 3D positions
const char* vertex_shader_src =
 "#version 330 core\n"
 "layout(location = 0) in vec3 aPos;\n"
 "void main() { gl_Position = vec4(aPos, 1.0); }\n";

// Fragment shader that outputs a solid color
const char* fragment_shader_src =
 "#version 330 core\n"
 "out vec4 FragColor;\n"
 "void main() { FragColor = vec4(1.0, 0.0, 1.0, 1.0); }\n";

// Utility function to compile a shader from source
GLuint compile_shader(GLenum type, const char* src) {
 GLuint shader = glCreateShader(type);
 glShaderSource(shader, 1, &src, NULL);
 glCompileShader(shader);
 return shader;
}

int main() {
    // Initialize GLFW and create a window with OpenGL 3.3 core profile
 glfwInit();
 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 GLFWwindow* window = glfwCreateWindow(800, 600, "GEOMOS TRIANGLE", NULL, NULL);
 if (!window) { fprintf(stderr, "Failed to create window\n"); return -1; }

 glfwMakeContextCurrent(window);
 if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
 fprintf(stderr, "Failed to load OpenGL\n"); return -1;
 }

 printf("OpenGL %s\n", glGetString(GL_VERSION));

 GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
 GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
 GLuint program = glCreateProgram();
 glAttachShader(program, vs);
 glAttachShader(program, fs);
 glLinkProgram(program);
 glUseProgram(program);

 // Define a simple triangle in normalized device coordinates
 float verts[] = {
 -0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 0.0f, 0.5f, 0.0f
 };

 // Create and setup vertex buffer and vertex array objects
 GLuint vao, vbo;
 glGenVertexArrays(1, &vao);
 glGenBuffers(1, &vbo);
 glBindVertexArray(vao);
 glBindBuffer(GL_ARRAY_BUFFER, vbo);
 glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
 glEnableVertexAttribArray(0);

 // Main render loop
 while (!glfwWindowShouldClose(window)) {
 glClear(GL_COLOR_BUFFER_BIT);
 glDrawArrays(GL_TRIANGLES, 0, 3);
 glfwSwapBuffers(window);
 glfwPollEvents();
 }

 glfwDestroyWindow(window);
 glfwTerminate();
 return 0;
}
