#include <iostream>
#include <vector>
#include <memory>
#include "window_manager.h"

void blockingTopDemo(std::shared_ptr<Window> window);

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int       nCmdShow
)
{
	try {
		auto on_close = [](
			WindowManager::WinHnd wnd, unsigned int msg, WindowManager::MsgParam) -> long {
			DestroyWindow(wnd);
			return 0;
		};
		int windows_cnt = 3;
		std::vector<std::shared_ptr<Window>> windows(windows_cnt);
		for (int i = 0, j = 1; i < windows_cnt - 1; ++i) {
			//create and open the window
			windows[i] = i == 0 ? WindowManager::buildWindow("Close me to quit... ", 500, 500) :
				WindowManager::buildWindow("Window ", 500, 500);
			WindowManager::setWindowPosition(windows[i], 20 + j * 20, 20 + j * 20);
			//set on_close as handler for message WM_CLOSE for window[i]
			WindowManager::setMessageHandler(windows[i], WM_CLOSE, on_close);
			j += 10;
		}
		//quit from app if windows[0] is closed
		WindowManager::setMessageHandler(windows[0], WM_DESTROY, [](
			WindowManager::WinHnd wnd, unsigned int msg, WindowManager::MsgParam) -> long {
			PostQuitMessage(0);
			return 0;
		});

		windows[windows_cnt - 1] = WindowManager::buildWindow(
			"Blocking top window: minimize me to continue...", 500, 500);
		WindowManager::setWindowPosition(windows[windows_cnt - 1], 600, 300);
		//demonstration of placement window on top: blocking and non-blocking ways
		blockingTopDemo(windows[windows_cnt - 1]);

		//place windows[0] above windows[1]
		WindowManager::setWindowAfter(windows[1], windows[0]);
		WindowManager::getMessage();
	}
	catch (std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

void blockingTopDemo(std::shared_ptr<Window> fixed) {
	WindowManager::setBlockingTop(fixed);
	auto release_focus = [](
		WindowManager::WinHnd wnd, unsigned int msg, WindowManager::MsgParam msg_param) -> long {
		//release blocking focus if window was minimized
		if (msg_param == SIZE_MINIMIZED) {
			WindowManager::releaseBlockingTop();
			std::shared_ptr<Window> new_top = WindowManager::buildWindow(
				"Non-blocking top: i always above...", 600, 600);
			//set new_top to top of Z-order
			WindowManager::setWindowTop(new_top);
		}
		return 0;
	};
	auto release_focus_on_close = [](
		//release fixed focus if window was close
		WindowManager::WinHnd wnd, unsigned int msg, WindowManager::MsgParam msg_param) -> long {
		WindowManager::releaseBlockingTop();
		DestroyWindow(wnd);
		return 0;
	};
	WindowManager::setMessageHandler(fixed, WM_SIZE, release_focus);
	WindowManager::setMessageHandler(fixed, WM_CLOSE, release_focus_on_close);
}