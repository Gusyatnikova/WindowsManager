#include <cstring>
#include <exception>
#include <windows.h>
#include "window.h"

Window::Window(const std::string& title, const std::string& class_name,
	int width, int height, int x_coord, int y_coord, int nCmdShow) :
	_x_coord(x_coord), _y_coord(y_coord), _width(width), _height(height) {
	_class_name = class_name;
	_title = title;
	_nCmdShow = nCmdShow;
	_hwnd = CreateWindowEx(
	WS_EX_CLIENTEDGE,
		_class_name.c_str(),
		_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		_x_coord, _y_coord,
		_width, _height,
		NULL, NULL, GetModuleHandle(nullptr), NULL);
	if (nullptr == _hwnd) throw new std::runtime_error("Cannot create window");
}

Window::~Window() {}

HWND Window::getHwnd() const { return _hwnd; }

void Window::setPosition(int x, int y) {
	_x_coord = x;
	_y_coord = y;
}

void Window::setSize(int x, int y) {
	_width = x;
	_height = y;
}

