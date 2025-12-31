#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <stdbool.h>

#include "bitmap.h"

typedef struct {
	HWND handle;
	WNDCLASS class;
	RECT rectangle;
	HDC hDeviceContext;
	HDC hBufferDeviceContext;
	HBITMAP hBufferBitmap;
} Window;

Window create_window(HINSTANCE hInstance, Bitmap_t bitmap, LPCSTR strTitle, LPCSTR strClassName, LRESULT CALLBACK pProcedureCallback(HWND, UINT, WPARAM, LPARAM), bool isDoubleBuffered);

void blit_bitmap_to_window(Bitmap_t bitmap, Window window);

void poll_window_events(HWND hWindow);

void destroy_window(Window window);

#endif
