#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define SYL_IMPLEMENTATION
#include <Sylvester/sylvester.h>

int window_resized;
int window_width;
int window_height;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	window_resized = 1;
	window_width = width;
	window_height = height;
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
	glShaderSource(vs, 1, (const GLchar * const*) &vs_source.data, NULL);
	glCompileShader(vs);

	int success;
	char infolog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if ( !success ) {
		glGetShaderInfoLog(vs, 512, NULL, infolog);
		fprintf(stderr, "shader compilation failed: %s\n", infolog);
	}

	int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, (const GLchar * const*) &fs_source.data, NULL);
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

void handle_window_resize(smat4 *model)
{
	if (window_resized == 1)
		window_resized = 0;
	else
		return;

	float aspect_ratio = (float)window_width / (float)window_height;

	*model = s_mat4_identity();
	if ( aspect_ratio > 1 )
		*model = s_mat4_scale(*model, SVEC3(1 / aspect_ratio, 1.0f, 1.0f));
	else
		*model = s_mat4_scale(*model, SVEC3(1, aspect_ratio, 1.0f));
}

/* float verticies[] = { */
/* 	300.0f, 300.0f, 0.0f, */
/* 	300.0f, 100.0f, 0.0f, */
/* 	100.0f, 100.0f, 0.0f, */
/* 	100.0f, 300.0f, 0.0f, */
/* }; */

/* unsigned int indicies[] = { */
/* 	2, 1, 0, */
/* 	0, 3, 2, */
/* }; */

#define VERTICIES_SIZE 1024 * 12
#define INDICIES_SIZE 1024 * 6

float verticies[VERTICIES_SIZE];
int vertexIndex;

unsigned int indicies[INDICIES_SIZE];
int indiciesIndex;

#if 0
void push_quad(int x, int y, int width, int height)
{
	float *vertCursor = verticies + vertexIndex;
	unsigned int *indiciesCursor = indicies + indiciesIndex;

	vertCursor[0] = x + width;
	vertCursor[1] = y;
	vertCursor[2] = 0.0f;

	vertCursor[3] = x + width;
	vertCursor[4] = y - height;
	vertCursor[5] = 0.0f;

	vertCursor[6] = x;
	vertCursor[7] = y - height;
	vertCursor[8] = 0.0f;

	vertCursor[9] = x;
	vertCursor[10] = y;
	vertCursor[11] = 0.0f;

	indiciesCursor[0] = lastIndicie + 2;
	indiciesCursor[1] = lastIndicie + 1;
	indiciesCursor[2] = lastIndicie + 0;
	indiciesCursor[3] = lastIndicie + 0;
	indiciesCursor[4] = lastIndicie + 3;
	indiciesCursor[5] = lastIndicie + 2;

	vertexIndex += 12;
	indiciesIndex += 6;
}

#else
void push_quad(int x, int y, int width, int height)
{
	float *vertCursor = verticies + vertexIndex;
	unsigned int *indiciesCursor = indicies + indiciesIndex;
	int elementIndex = vertexIndex / 4 + (vertexIndex / 12);

	vertCursor[0] = x + width;
	vertCursor[1] = y;
	vertCursor[2] = 0.0f;

	vertCursor[3] = x + width;
	vertCursor[4] = y - height;
	vertCursor[5] = 0.0f;

	vertCursor[6] = x;
	vertCursor[7] = y - height;
	vertCursor[8] = 0.0f;

	vertCursor[9] = x;
	vertCursor[10] = y;
	vertCursor[11] = 0.0f;

	indiciesCursor[0] = elementIndex + 0;
	indiciesCursor[1] = elementIndex + 1;
	indiciesCursor[2] = elementIndex + 2;
	indiciesCursor[3] = elementIndex + 2;
	indiciesCursor[4] = elementIndex + 3;
	indiciesCursor[5] = elementIndex + 0;

	vertexIndex += 12;
	indiciesIndex += 6;
}
#endif

void clear_buffer()
{
	vertexIndex = 0;
	indiciesIndex = 0;
	memset(verticies, 0, VERTICIES_SIZE);
	memset(indicies, 0, VERTICIES_SIZE);
}

/* int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int cmdshow) */
int main(int argc, char **argv)
{
	GLFWwindow *window;
	int shader_program;
	unsigned int VAO, VBO, EBO;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_width = SRC_WIDTH;
	window_height = SRC_HEIGHT;

	window = glfwCreateWindow(window_width, window_height, "opengl", NULL, NULL);
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
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	smat4 model = s_mat4_identity();
	smat4 view = s_mat4_identity();
	smat4 proj = s_mat4_identity();

	int modelLoc = glGetUniformLocation(shader_program, "model");
	int viewLoc = glGetUniformLocation(shader_program, "view");
	int projLoc = glGetUniformLocation(shader_program, "proj");

	proj = s_mat4_ortho_rh(0.0f, window_width, 0.0f, window_height, 0.0f, 100.0f);

	while ( !glfwWindowShouldClose(window) ) {
		process_input(window);
		handle_window_resize(&model);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		push_quad(200.0f, 200.0f, 100.0f, 100.0f);
		push_quad(500.0f, 600.0f, 100.0f, 100.0f);
		push_quad(100.0f, 600.0f, 100.0f, 100.0f);
		push_quad(200.0f, 400.0f, 100.0f, 100.0f);

		glUseProgram(shader_program);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexIndex, verticies, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indiciesIndex, indicies, GL_STATIC_DRAW);

		glUniformMatrix4fv(modelLoc, 1, 0 , (const GLfloat *) &model);
		glUniformMatrix4fv(viewLoc, 1, 0 , (const GLfloat *) &view);
		glUniformMatrix4fv(projLoc, 1, 0 , (const GLfloat *) &proj);

		/* glDrawArrays(GL_TRIANGLES, 0, 3); */


		glDrawElements(GL_TRIANGLES, indiciesIndex, GL_UNSIGNED_INT, 0);

		clear_buffer();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;
}
