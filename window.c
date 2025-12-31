#include "window.h"

Window create_window(HINSTANCE hInstance, Bitmap_t bitmap, LPCSTR strTitle, LPCSTR strClassName, LRESULT CALLBACK pProcedureCallback(HWND, UINT, WPARAM, LPARAM), bool isDoubleBuffered) {
	Window window = {0};

	window.class.lpfnWndProc = pProcedureCallback;
	window.class.hInstance = hInstance;
	window.class.lpszClassName = strClassName;
	window.class.hCursor = LoadCursor(0, IDC_ARROW);
	window.class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(0));
	RegisterClass(&window.class);

	window.handle = CreateWindowEx(
		0,
		window.class.lpszClassName,
		strTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		bitmap.width,
		bitmap.height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	window.hDeviceContext = GetDC(window.handle);
	if (isDoubleBuffered) {
		BITMAPINFO bitmapInfo = {0};
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = bitmap.width;
		bitmapInfo.bmiHeader.biHeight = -bitmap.height;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = bitmap.channelCount * 8; /* Assuming 8 bits in a byte */
		bitmapInfo.bmiHeader.biCompression = BI_RGB;

		window.hBufferDeviceContext = CreateCompatibleDC(window.hDeviceContext);
		window.hBufferBitmap = CreateDIBSection(window.hDeviceContext, &bitmapInfo, DIB_RGB_COLORS, (void **) &(bitmap.pData), NULL, 0);

		SelectObject(window.hBufferDeviceContext, window.hBufferBitmap);
	}

	GetClientRect(window.handle, &(window.rectangle));
	ShowWindow(window.handle, SW_SHOW);
	
	return window;
}

void blit_bitmap_to_window(Bitmap_t bitmap, Window window) {
	BITMAPINFO bitmapInfo = {0};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = bitmap.width;
	bitmapInfo.bmiHeader.biHeight = -bitmap.height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = bitmap.channelCount * 8;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	SetDIBits(window.hBufferDeviceContext, window.hBufferBitmap, 0, bitmap.height, bitmap.pData, &bitmapInfo, DIB_RGB_COLORS);
	BitBlt(window.hDeviceContext, 0, 0, window.rectangle.right, window.rectangle.bottom, window.hBufferDeviceContext, 0, 0, SRCCOPY);
}

void poll_window_events(HWND hWindow) {
	MSG message = {0};
	while (PeekMessage(&message, hWindow, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void destroy_window(Window window) {
	ReleaseDC(window.handle, window.hDeviceContext);
	ReleaseDC(window.handle, window.hBufferDeviceContext);
	DeleteObject(window.hBufferBitmap);
	UnregisterClass(window.class.lpszClassName, window.class.hInstance);
}
