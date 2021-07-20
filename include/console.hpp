#pragma once

#include <cstdio>
#include <consoleapi.h>
#include <consoleapi2.h>

#define PRINT_FILE_CONSOLE(__FMT__, ...)												\
	if (const auto file = console::file())									\
	{																					\
		std::fprintf(file, __FMT__, __VA_ARGS__);										\
		std::fflush(file);																\
	}																					\
																						\
	std::printf(__FMT__, __VA_ARGS__)

#ifdef DEBUG
	#define PRINT_DEBUG(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ DEBUG ][" __FUNCTION__ "]: " __FMT__ "\n", __VA_ARGS__)

	#define PRINT_INFO(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ INFO ][" __FUNCTION__ "]: " __FMT__ "\n", __VA_ARGS__)

	#define PRINT_WARNING(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ WARNING ][" __FUNCTION__ "]: " __FMT__ "\n", __VA_ARGS__)

	#define PRINT_ERROR(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ ERROR ][" __FUNCTION__ "]: " __FMT__ "\n", __VA_ARGS__)
#else
	#define PRINT_DEBUG(__FMT__, ...)

	#define PRINT_INFO(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ INFO ]: " __FMT__ "\n", __VA_ARGS__)

	#define PRINT_WARNING(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ WARNING ]: " __FMT__ "\n", __VA_ARGS__)

	#define PRINT_ERROR(__FMT__, ...)													\
		PRINT_FILE_CONSOLE("[ ERROR ]: " __FMT__ "\n", __VA_ARGS__)
#endif

	class console final
	{
		public:
			static void init();
			static ::_iobuf* file();

		private:
			static ::_iobuf* file_;
	};
