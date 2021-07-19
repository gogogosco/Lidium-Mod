#include <utils/console/console.hpp>

namespace mm_template::utils
{
	void console::init()
	{
		console::file_ = std::fopen("console.log", "wb");

		::AllocConsole();
		::SetConsoleTitleA("Awakened DNF Console");

		std::freopen("CONOUT$", "w", stdout);
		std::freopen("CONIN$", "r", stdin);
	}

	::_iobuf* console::file()
	{
		return console::file_;
	}

	::_iobuf* console::file_;
}
