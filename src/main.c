#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <windows.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
	if ( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ) {
		glfwSetWindowShouldClose(window, 1);
	}
}

const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

struct file {
	char *data;
	size_t size;
};

struct file read_file(char *path) {
	struct file result = {0};
	size_t alloc_size = 0;

	FILE *file = fopen(path, "r");
	if( !file ) {
		result.data = 0;
		result.size = 0;
		return result;
	}

	fseek(file, 0, SEEK_END);
	result.size = ftell(file);
	fseek(file, 0, SEEK_SET);

	alloc_size = result.size + 1;

	result.data = (char*)malloc(result.size + 1);
	if( result.data && result.size ) {
		fread(result.data, result.size, 1, file);
		((char*)result.data)[result.size] = '\0';
		result.size = alloc_size;
	} else {
		result.data = 0;
		result.size = 0;
	}

	fclose(file);

	return result;
}

// =======================
// Shaders
// Return shader program id
int compile_link_shaders() {
	struct file vs_source;
	struct file fs_source;
	vs_source = read_file("../src/vertex.shader");
	fs_source = read_file("../src/fragment.shader");

	if ( vs_source.size == 0 || fs_source.size == 0) {
		fprintf(stderr, "Unable to read file\n");
		return -1;
	}

	int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source.data, NULL);
	glCompileShader(vs);

	int success;
	char infolog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if ( !success ) {
		glGetShaderInfoLog(vs, 512, NULL, infolog);
		fprintf(stderr, "shader compilation failed: %s\n", infolog);
	}

	int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source.data, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if ( !success ) {
		glGetShaderInfoLog(fs, 512, NULL, infolog);
		fprintf(stderr, "shader compilation failed: %s\n", infolog);
	}

	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if ( !success ) {
		glGetProgramInfoLog(shader_program, 512, NULL, infolog);
		fprintf(stderr, "shader program cannot link: %s\n", infolog);
	}
	glDeleteShader(vs);
	glDeleteShader(fs);

	free(vs_source.data);
	free(fs_source.data);

	return shader_program;
}

float verticies[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int cmdshow)
{
	GLFWwindow *window;
	int shader_program;
	unsigned int VAO, VBO;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "opengl", NULL, NULL);
	if ( window == NULL ) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ) {
		fprintf(stderr, "Failed to initialize glad\n");
		glfwTerminate();
		return -1;
	}

	shader_program = compile_link_shaders();

	// =============
	// VAO VBO

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while ( !glfwWindowShouldClose(window) ) {
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;
}
