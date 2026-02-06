#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#include "clock.h"
#include "window.h"
#include "draw.h"
#include "clip.h"
#include "wind.h"
#include "render.h"
#include "pipeline.h"

#include <stb/stb_image.h>

/* In seconds */
#define TARGET_FPS 60
/* In milliseconds */
#define TARGET_FRAME_TIME (1000.0 / (double) (TARGET_FPS))


typedef struct {
	bool hasMoved;
	Vec3f position;
} Camera;
Camera camera = {true, {0.0, 0.0, 1.0}};
bool hasResized = true;

static bool isRunning = true;
Window mainWindow;
Bitmap_t bitmap;

static LRESULT CALLBACK window_procedure_callback(HWND hWindow, UINT messageCode, WPARAM wordParameter, LPARAM longParameter) {
	switch (messageCode) {
		case WM_ERASEBKGND:
			return 1;
		case WM_SIZE:
			GetClientRect(hWindow, &(mainWindow.rectangle));

			DeleteObject(mainWindow.hBufferBitmap);
			WORD newWidth = LOWORD(longParameter);
			WORD newHeight = HIWORD(longParameter);
			mainWindow.hBufferBitmap = CreateCompatibleBitmap(mainWindow.hDeviceContext, newWidth, newHeight);
			SelectObject(mainWindow.hBufferDeviceContext, mainWindow.hBufferBitmap);

			bitmap.width = newWidth;
			bitmap.height = newHeight;
			bitmap.stride = calculate_stride(bitmap.width, bitmap.channelCount);
			bitmap.pData = realloc(bitmap.pData, bitmap.stride * bitmap.height * sizeof(uint8_t));

			hasResized = true;

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			isRunning = false;

			return 0;
		default:
			return DefWindowProc(hWindow, messageCode, wordParameter, longParameter);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pStrArgs, int windowState) {
	/* Must be 4 for padding requirements */
	bitmap = create_bitmap(640, 640, 4);

	mainWindow = create_window(hInstance, bitmap, TEXT("3D Rasterizer"), TEXT("Rasterizer3D"), window_procedure_callback, true);

	double currentTime, lastTime, deltaTime, FPS = 0.0;
	init_clock();
	currentTime = get_time();

	MSG message = {0};
	while (isRunning) {
		lastTime = currentTime;

		poll_window_events(mainWindow.handle);

		const float speed = 0.001;
		/* Virtual key codes (VK_-prefixed macros) don't exist for letters and numbers unfortunately-
		https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes */
		/* W: Forward */
		if (GetAsyncKeyState(0x57) & 0x8000) {
			camera.position[2] += -speed * deltaTime;
			camera.hasMoved = true;
		}
		/* S: Backward */
		if (GetAsyncKeyState(0x53) & 0x8000) {
			camera.position[2] += speed * deltaTime;
			camera.hasMoved = true;
		}
		/* D: Right */
		if (GetAsyncKeyState(0x44) & 0x8000) {
			camera.position[0] += speed * deltaTime;
			camera.hasMoved = true;
		}
		/* A: Left */
		if (GetAsyncKeyState(0x41) & 0x8000) {
			camera.position[0] += -speed * deltaTime;
			camera.hasMoved = true;
		}
		/* E: Up */
		if (GetAsyncKeyState(0x45) & 0x8000) {
			camera.position[1] += speed * deltaTime;
			camera.hasMoved = true;
		}
		/* Q: Down */
		if (GetAsyncKeyState(0x51) & 0x8000) {
			camera.position[1] += -speed * deltaTime;
			camera.hasMoved = true;
		}

		/* Only scale matrix used as model */
		Mat4x4f projection, view, projectionView, model, rotation, scale, translation, MVP;
		bool mustUpdateProjectionView = false;
		if (hasResized) {
			mat4x4f_perspective(M_PI * 0.5, (float) bitmap.width / (float) bitmap.height, 0.1, 2.0, projection);
			hasResized = false;
			mustUpdateProjectionView = true;
		}
		if (camera.hasMoved) {
			mat4x4f_camera(camera.position, (Vec3f) {1.0, 0.0, 0.0}, (Vec3f) {0.0, 1.0, 0.0}, (Vec3f) {0.0, 0.0, 1.0}, view);
			camera.hasMoved = false;
			mustUpdateProjectionView = true;
		}
		if (mustUpdateProjectionView) {
			mat4x4f_mul_mat4x4f(projection, view, projectionView);
			mustUpdateProjectionView = false;
		}
		mat4x4f_scale((Vec3f) {0.2, 0.2, 0.2}, scale);
		mat4x4f_rotation_y(currentTime * 0.001, rotation);
		mat4x4f_mul_mat4x4f(scale, rotation, model);
		mat4x4f_mul_mat4x4f(projectionView, model, MVP);

		clear_bitmap(bitmap);

		const float positions[] = {
			-1.0, 1.0, 0.0, /* top-left */
			1.0, 1.0, 0.0, /* top-right */
			-1.0, -1.0, 0.0, /* bottom-left */
			1.0, -1.0, 0.0 /* bottom-right */
		};
		const float color[] = {
			1.0, 0.0, 0.0, /* top-left */
			0.0, 1.0, 0.0, /* top-right */
			0.0, 0.0, 1.0, /* bottom-left */
			1.0, 1.0, 0.0 /* bottom-right */
		};
		const unsigned int indices[] = {
			0, 1, 2,
			1, 3, 2
		};
		const unsigned int polygonCount = 2;
		for (unsigned int i = 0; i < polygonCount; i++) {
			currentTriangle.positions[0][0] = positions[indices[i * 3 + 0] * 3 + 0];
			currentTriangle.positions[0][1] = positions[indices[i * 3 + 0] * 3 + 1];
			currentTriangle.positions[0][2] = positions[indices[i * 3 + 0] * 3 + 2];
			currentTriangle.positions[0][3] = 1.0;
			currentTriangle.positions[1][0] = positions[indices[i * 3 + 1] * 3 + 0];
			currentTriangle.positions[1][1] = positions[indices[i * 3 + 1] * 3 + 1];
			currentTriangle.positions[1][2] = positions[indices[i * 3 + 1] * 3 + 2];
			currentTriangle.positions[1][3] = 1.0;
			currentTriangle.positions[2][0] = positions[indices[i * 3 + 2] * 3 + 0];
			currentTriangle.positions[2][1] = positions[indices[i * 3 + 2] * 3 + 1];
			currentTriangle.positions[2][2] = positions[indices[i * 3 + 2] * 3 + 2];
			currentTriangle.positions[2][3] = 1.0;
			render_triangle(currentTriangle.positions, MVP, bitmap);
		}

		blit_bitmap_to_window(bitmap, mainWindow);
		RedrawWindow(mainWindow.handle, NULL, NULL, RDW_INVALIDATE);

		currentTime = get_time();
		deltaTime = currentTime - lastTime;
		FPS = deltaTime / TARGET_FRAME_TIME * TARGET_FPS;

		if (deltaTime < TARGET_FRAME_TIME) {
			while (deltaTime < TARGET_FRAME_TIME) {
				deltaTime = get_time() - lastTime;
			}
		}
	}

	free(bitmap.pData);

	return EXIT_SUCCESS;
}
