#pragma once
#include <string>

class Window {
private:
	int _x_coord;
	int _y_coord;
	int _width;
	int _height;
	std::string _class_name;
	std::string _title;
	int _nCmdShow;
	HWND _hwnd;
public:
	~Window();
	Window(const std::string& wnd_title, const std::string& class_name,
		int width, int height, int x_coord, int y_coord, int nCmdShow);
	HWND getHwnd() const;
	void setPosition(int x, int y);
	void setSize(int x, int y);
};
