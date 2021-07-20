#include <console.hpp>
#include <cstdio>
#include <consoleapi2.h>
#include <consoleapi.h>

	void console::init()
	{
		console::file_ = std::fopen("console.log", "wb");

		AllocConsole();
		SetConsoleTitleA("MapleStory Console");

		std::freopen("CONOUT$", "w", stdout);
		std::freopen("CONIN$", "r", stdin);
	}

	::_iobuf* console::file()
	{
		return console::file_;
	}

	::_iobuf* console::file_;

