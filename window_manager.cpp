#include <map>
#include <vector>
#include <exception>
#include <string>
#include <windows.h>
#include "window_manager.h"

//Window base class artefacts
static WNDCLASSEX base_window_class;
static const std::string base_window_class_name = "BaseWindowClass";
static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
//Window manager artefacts 
static std::vector<std::shared_ptr<Window>> wnd_creation_order;
static std::map<HWND, WindowManager::MsgToHandler> msg_handlers;

void initBaseWindowClass(WNDCLASSEX& wc, const std::string& class_name) {
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = class_name.c_str();
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		throw new std::runtime_error("Cannot register base window class");
	}
}

std::shared_ptr<Window> getWindow(std::string wnd_title, int nCmdShow,
	int width, int height, int x_coord, int y_coord) {
	if (wnd_creation_order.empty()) {
		initBaseWindowClass(base_window_class, base_window_class_name);
	}
	wnd_creation_order.push_back(
		std::make_shared<Window>(Window(wnd_title, base_window_class_name,
			width, height, x_coord, y_coord, SW_SHOWNORMAL)));
	ShowWindow((*std::prev(wnd_creation_order.end()))->getHwnd(), SW_SHOWNORMAL);
	return *std::prev(wnd_creation_order.end());
}

std::shared_ptr<Window> WindowManager::buildWindow(const std::string& title, int w, int h) {
	return getWindow(title, SW_SHOWNORMAL, w, h, CW_USEDEFAULT, CW_USEDEFAULT);
}

void WindowManager::setWindowPosition(std::shared_ptr<Window> window, int x, int y) {
	window->setPosition(x, y);
	SetWindowPos(window->getHwnd(), HWND_NOTOPMOST, x, y, 0, 0, SWP_NOREPOSITION | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowManager::setWindowSize(std::shared_ptr<Window> window, int x, int y) {
	window->setSize(x, y);
	SetWindowPos(window->getHwnd(), HWND_NOTOPMOST, 0, 0, x, y, SWP_NOREPOSITION | SWP_NOMOVE | SWP_NOACTIVATE);
}

void WindowManager::setWindowTop(const std::shared_ptr<Window> window) {
	SetWindowPos(window->getHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetActiveWindow(window->getHwnd());
}

void WindowManager::setBlockingTop(const std::shared_ptr<Window> window) {
	for (auto window_pair : msg_handlers) {
		if (window_pair.first != window->getHwnd()) {
			EnableWindow(window_pair.first, false);
		}
	}
	SetWindowPos(window->getHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void WindowManager::releaseBlockingTop() {
	for (auto window_pair : msg_handlers) {
		EnableWindow(window_pair.first, true);
	}
}

void WindowManager::setWindowBottom(const std::shared_ptr<Window> window) {
	SetWindowPos(window->getHwnd(), HWND_BOTTOM, 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowManager::setWindowAfter(const std::shared_ptr<Window> window,
	const std::shared_ptr<Window> set_after) {
	SetWindowPos(window->getHwnd(), set_after->getHwnd(), 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowManager::setMessageHandler(
	const std::shared_ptr<Window> hwnd, unsigned int msg_id, const MsgHandler handler) {
	msg_handlers[hwnd->getHwnd()][msg_id] = handler;
}

WindowManager::MsgHandler getMessageHandler(
	const HWND hwnd, unsigned int msg_id) {
	auto window_handler = msg_handlers.find(hwnd);
	if (msg_handlers.end() != window_handler) {
		auto message = (window_handler->second).find(msg_id);
		if ((window_handler->second).end() != message) return message->second;
	}
	return nullptr;
}

WindowManager::MsgHandler WindowManager::getMessageHandler(const std::shared_ptr<Window> window, unsigned int msg_id) {
	return getMessageHandler(window->getHwnd(), msg_id);
}

void WindowManager::getMessage() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	WindowManager::MsgHandler msg_handler = getMessageHandler(hwnd, msg);
	if (!msg_handler) {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	else {
		return msg_handler(hwnd, msg, wParam);
	}
}
