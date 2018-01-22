#pragma once

#include "windows.h"
class Window {
public:
	HWND handle = 0;
	HINSTANCE inst = 0;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		Window* _this = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		/*case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			EndPaint(hwnd, &ps);
		}
		return 0;*/

		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
        static const wchar_t* getWindowClassName() {
                //constexpr wchar_t* name = L"Antilatency";
                return L"Antilatency";
        }
	static void registerWindowClass(HINSTANCE instance) {
		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = instance;
		wc.lpszClassName = getWindowClassName();
		RegisterClass(&wc);
	}
	
	Window(HINSTANCE instance) {

		inst = instance;

		registerWindowClass(instance);

		handle	= CreateWindowEx(
			0,                              // Optional window styles.
			getWindowClassName(),                     // Window class
			L"Antilatency vulkan test",    // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

											// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window    
			NULL,       // Menu
			instance,  // Instance handle
			NULL        // Additional application data
		);


		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
	}

	 
	void show() {
		ShowWindow(handle, SW_SHOWDEFAULT);
	}
	
	static bool processMessages() {
		MSG msg = {};
		while (PeekMessage(&msg, NULL, 0, 0, true))
		{
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	

	// Create the window.

	


};
