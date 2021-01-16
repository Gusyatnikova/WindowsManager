#pragma once
#include <map>
#include <memory>
#include <windows.h>
#include "window.h"

namespace WindowManager {
	using WinHnd = HWND;
	using MsgParam = WPARAM;
	using MsgHandler = long (*) (HWND window, unsigned int msg, MsgParam msg_param);
	using MsgToHandler = std::map<unsigned int, MsgHandler>;

	std::shared_ptr<Window> buildWindow(const std::string& title, int w, int h);
	void setMessageHandler(const std::shared_ptr<Window> hwnd, unsigned int msg_id,const MsgHandler mh);
	MsgHandler getMessageHandler(const std::shared_ptr<Window> window, unsigned int msg_id);
	void getMessage();
	void setWindowPosition(std::shared_ptr<Window> window, int x, int y);
	void setWindowSize(std::shared_ptr<Window> window, int x, int y);
	void setWindowAfter(const std::shared_ptr<Window> window, const std::shared_ptr<Window> hwnd_insert_after);
	void setWindowTop(const std::shared_ptr<Window> window);
	void setWindowBottom(const std::shared_ptr<Window> window);
	void setBlockingTop(const std::shared_ptr<Window> window);
	void releaseBlockingTop();
}

