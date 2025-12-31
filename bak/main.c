#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#include "clock.h"
#include "window.h"
#include "draw.h"
#include "glutils.h"
#include "renderable.h"
#include "texture.h"

#include <cgmath.h>
#include <glfl/glfl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

/* In degrees */
#define FOV 90.0

/* In seconds */
#define TARGET_FPS 60
/* In milliseconds */
#define TARGET_FRAME_TIME (1000.0 / (double) (TARGET_FPS))
#define FPS_DISPLAY_STRING_LENGTH 6

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

bool isRunning = true;

struct Window GLWindow;
struct Window *pSelectedWindow;
struct Rectangle square;

static LRESULT CALLBACK gl_window_procedure_callback(HWND hWindow, UINT messageCode, WPARAM wordParameter, LPARAM longParameter) {
	switch (messageCode) {
		case WM_ERASEBKGND:
			return 1;
		case WM_SIZE:
			GetClientRect(hWindow, &(pSelectedWindow->rectangle));

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			FreeConsole();
			isRunning = false;

			return 0;
		default:
			return DefWindowProc(hWindow, messageCode, wordParameter, longParameter);
	}
}

char *read_file(const char *pFileName, size_t *pFileSize) {
	FILE* pFile = fopen(pFileName, "rb");
	char *pFileContents;

	if (pFile == NULL) {
		return NULL;
	}

	fseek(pFile, 0, SEEK_END);

	if (pFileSize) {
		*pFileSize = ftell(pFile);
		rewind(pFile);
		pFileContents = (char *) calloc(1, *pFileSize + 1);
		if (pFileContents == NULL) {
			fclose(pFile);
			return NULL;
		}
		fread(pFileContents, 1, *pFileSize, pFile);
	} else {
		long unsigned int fileSize;

		fileSize = ftell(pFile);
		rewind(pFile);
		pFileContents = (char *) calloc(1, fileSize + 1);
		if (pFileContents == NULL) {
			fclose(pFile);
			return NULL;
		}
		fread(pFileContents, 1, fileSize, pFile);		
	}

	fclose(pFile);

	return pFileContents;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pStrArgs, int windowState) {
	pSelectedWindow = &GLWindow;
	GLWindow = create_window(hInstance, 400, 400, TEXT("OpenGL"), TEXT("ClassGLWindow"), gl_window_procedure_callback, false);
	glfl_init();
	HGLRC hGLContext = create_gl_context(GLWindow.hDeviceContext);
	glfl_set_gl_function_pointers();
	glfl_finalize();

	GLuint vertexShaderID, fragmentShaderID, shaderProgramID;
	char* strShaderSource = read_file("shader.vert.glsl", NULL);
	create_shader(strShaderSource, GL_VERTEX_SHADER, &vertexShaderID);
	free(strShaderSource);
	strShaderSource = read_file("shader.frag.glsl", NULL);
	create_shader(strShaderSource, GL_FRAGMENT_SHADER, &fragmentShaderID);
	free(strShaderSource);
	create_shader_program(vertexShaderID, fragmentShaderID, &shaderProgramID);
	glUseProgram(shaderProgramID);
	glUniform1i(glGetUniformLocation(shaderProgramID, "mau5tex"), 0);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

GLfloat vertices[] = {
    // positions          // colors           // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left 
};
GLuint arrIndices[] = {
	0, 1, 3,
	1, 2, 3
};
VertexAttribute arrVertexAttributes[] = {
	{3, GL_FLOAT, (GLvoid *) 0},
	{3, GL_FLOAT, (GLvoid *) (3 * sizeof(GLfloat))},
	{2, GL_FLOAT, (GLvoid *) (6 * sizeof(GLfloat))}
};

	GLuint VAO = create_vao();
	GLuint VBO = create_vbo(vertices, sizeof(vertices), arrVertexAttributes, ARRAY_LENGTH(arrVertexAttributes), 8 * sizeof(GLfloat));
	GLuint EBO = create_ebo(arrIndices, sizeof(arrIndices), ARRAY_LENGTH(arrIndices));

	//glEnable(GL_FRAMEBUFFER_SRGB);

	double currentTime, lastTime, deltaTime;
	init_clock();
	currentTime = get_time();

	GLfloat angle = 0.0;

	stbi_set_flip_vertically_on_load(true);
	Texture texture = create_texture("gallery/result.png", GL_TEXTURE0);

	glBindVertexArray(VAO);

	MSG message = {0};
	while (isRunning) {
		lastTime = currentTime;

		poll_window_events(GLWindow.handle);

		GLfloat matrix[16];
		cgm_mrotation(matrix, angle, 0.0, 0.0, 1.0);
		angle += 0.01;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "MVP"), 1, GL_FALSE, matrix);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		SwapBuffers(GLWindow.hDeviceContext);

		currentTime = get_time();
		deltaTime = currentTime - lastTime;

		if (deltaTime < TARGET_FRAME_TIME) {
			while (deltaTime < TARGET_FRAME_TIME) {
				deltaTime = get_time() - lastTime;
			}
		}
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgramID);

	wglMakeCurrent(GLWindow.hDeviceContext, NULL);
	wglDeleteContext(hGLContext);
	destroy_window(GLWindow);

	return 0;
}
