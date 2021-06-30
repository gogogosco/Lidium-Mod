#pragma once
#include <Shlwapi.h>
#include <ImageHlp.h>
#include <PsApi.h>
#include <dllmain.cpp>

#pragma comment(lib, "DbgHelp.lib")

#define LOG(...) fwprintf(sgLogFile, __VA_ARGS__)

char* gen_random(char* s, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		int randomChar = rand() % (26 + 26 + 10);
		if (randomChar < 26)
			s[i] = 'a' + randomChar;
		else if (randomChar < 26 + 26)
			s[i] = 'A' + randomChar - 26;
		else
			s[i] = '0' + randomChar - 26 - 26;
	}
	s[len] = 0;
	return s;
}

static void ReadKey(HKEY hKey, const char* ValueName, char* Buffer, DWORD size)
{
	DWORD dwType;
	LSTATUS ret = RegQueryValueExA(hKey, ValueName, NULL, &dwType, (LPBYTE)Buffer, &size);
	if (ret != ERROR_SUCCESS || dwType != REG_SZ)
		Buffer[0] = '\0';
}

std::wstring get_win_product_name()
{
	std::wstring result = L"Unknown Windows Product Name.";
	HKEY hkey;
	DWORD dwType, dwSize;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
	{
		wchar_t p_name_str[1024];

		dwType = REG_SZ;
		dwSize = sizeof(p_name_str);

		if (RegQueryValueExW(hkey, (L"ProductName"), NULL, &dwType, (PBYTE)&p_name_str, &dwSize) == ERROR_SUCCESS)
			result = p_name_str;

		RegCloseKey(hkey);
	}
	return result;
}

HMODULE GetThisDllHandle()
{
	MEMORY_BASIC_INFORMATION info;
	size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)GetThisDllHandle, &info, sizeof(info));
	if (len != sizeof(info)) return NULL;
	return len ? (HMODULE)info.AllocationBase : NULL;
}

BOOL ShowThreadStack(HANDLE hThread, CONTEXT* c, FILE* sgLogFile)
{
	STACKFRAME stFrame = { 0 };
	DWORD dwSymOptions, dwFrameNum = 0, dwMachine, dwOffsetFromSym = 0;
	IMAGEHLP_LINE Line = { 0 };
	IMAGEHLP_MODULEW Module = { 0 };
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	BYTE pbSym[sizeof(_IMAGEHLP_SYMBOLW) + 1024];
	BYTE pbSym2[sizeof(IMAGEHLP_SYMBOL) + 1024];
	_IMAGEHLP_SYMBOLW* pSym = (_IMAGEHLP_SYMBOLW*)&pbSym;
	wchar_t szUndecName[1024], szUndecFullName[1024];
	wchar_t msglog[4096];

	IMAGEHLP_SYMBOL* pSym2 = (IMAGEHLP_SYMBOL*)&pbSym2;

	if ( ! GetThreadContext( hThread, &c ) )
	{
		LOG(L"Cannot get thread context%d\n");
		return FALSE;
	}
	
	wsprintfW(msglog, L"Handler DLL base address: %08X\n", (unsigned int)GetThisDllHandle()); LOG(msglog);
	if (!SymInitialize(hProcess, NULL, TRUE))
	{
		wsprintfW(msglog, L"Cannot initialize symbol engine (%08X), trying again without invading process\n", GetLastError()); LOG(msglog);
		if (!SymInitialize(hProcess, NULL, FALSE))
		{
			wsprintfW(msglog, L"Cannot initialize symbol engine (%08X)\n", GetLastError()); LOG(msglog);
			return FALSE;
		}
	}

	dwSymOptions = SymGetOptions();
	dwSymOptions |= SYMOPT_LOAD_LINES;
	dwSymOptions &= ~SYMOPT_UNDNAME;
	SymSetOptions(dwSymOptions);

	stFrame.AddrPC.Mode = AddrModeFlat;
	dwMachine = IMAGE_FILE_MACHINE_I386;
	stFrame.AddrPC.Offset = c->Eip;
	stFrame.AddrStack.Offset = c->Esp;
	stFrame.AddrStack.Mode = AddrModeFlat;
	stFrame.AddrFrame.Offset = c->Ebp;
	stFrame.AddrFrame.Mode = AddrModeFlat;

	Module.SizeOfStruct = sizeof(Module);
	Line.SizeOfStruct = sizeof(Module);

	wsprintfW(msglog, L"\n--# FV EIP----- RetAddr- FramePtr StackPtr Symbol\n"); LOG(msglog);
	do
	{
		SetLastError(0);
		if (!StackWalk(dwMachine, hProcess, hThread, &stFrame, c, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
		{
			wsprintfW(msglog, L"Last error after stack walk finished: %08X", GetLastError()); LOG(msglog);
			break;
		}

		wsprintfW(msglog, L"\n%3d %c%c %08lx %08lx %08lx %08lx ",
			dwFrameNum, stFrame.Far ? 'F' : '.', stFrame.Virtual ? 'V' : '.',
			stFrame.AddrPC.Offset, stFrame.AddrReturn.Offset,
			stFrame.AddrFrame.Offset, stFrame.AddrStack.Offset); LOG(msglog);

		if (stFrame.AddrPC.Offset == 0)
		{
			wsprintfW(msglog, L"(-nosymbols-)\n"); LOG(msglog);
		}
		else
		{ // we seem to have a valid PC
			if (!SymGetSymFromAddr(hProcess, stFrame.AddrPC.Offset, &dwOffsetFromSym, pSym2))
			{
				if (GetLastError() != 487)
				{
					wsprintfW(msglog, L"Unable to get symbol from addr (%08X)\n", GetLastError()); LOG(msglog);
				}
			}
			else
			{
				UnDecorateSymbolNameW(pSym->Name, szUndecName, 1024, UNDNAME_NAME_ONLY);
				UnDecorateSymbolNameW(pSym->Name, szUndecFullName, 1024, UNDNAME_COMPLETE);
				wsprintfW(msglog, L"%s", szUndecName); LOG(msglog);
				if (dwOffsetFromSym) wsprintfW(msglog, L" %+ld bytes", (long)dwOffsetFromSym); LOG(msglog);
				wsprintfW(msglog, L"\n    Sig:  %s\n    Decl: %s\n", pSym->Name, szUndecFullName); LOG(msglog);
			}

			if (!SymGetLineFromAddr(hProcess, stFrame.AddrPC.Offset, &dwOffsetFromSym, &Line))
			{
				if (GetLastError() != 487)
				{
					wsprintfW(msglog, L"Unable to get line from addr (%08X)\n", GetLastError()); LOG(msglog);
				}
			}
			else
			{
				wsprintfW(msglog, L"    Line: %s(%lu) %+ld bytes\n", Line.FileName, Line.LineNumber, dwOffsetFromSym); LOG(msglog);
			}

			if (!SymGetModuleInfoW(hProcess, stFrame.AddrPC.Offset, &Module))
			{
				if (GetLastError() != 487)
				{
					wsprintfW(msglog, L"Unable to get module info (%08X)\n", GetLastError()); LOG(msglog);
				}
			}
			else
			{
				wchar_t ty[256];

				switch (Module.SymType)
				{
				case SymNone:
					wcscpy_s(ty, L"-nosymbols-");
					break;
				case SymCoff:
					wcscpy_s(ty, L"COFF");
					break;
				case SymCv:
					wcscpy_s(ty, L"CV");
					break;
				case SymPdb:
					wcscpy_s(ty, L"PDB");
					break;
				case SymExport:
					wcscpy_s(ty, L"-exported-");
					break;
				case SymDeferred:
					wcscpy_s(ty, L"-deferred-");
					break;
				case SymSym:
					wcscpy_s(ty, L"SYM");
					break;
				default:
					_snwprintf_s(ty, sizeof ty, L"symtype=%ld", (long)Module.SymType);
					break;

				}
				wsprintfW(msglog, L"    Mod:  %s[%s], base: %08lxh\n", Module.ModuleName, Module.ImageName, Module.BaseOfImage); LOG(msglog);
				wsprintfW(msglog, L"    Sym:  type: %s, file: %s\n", ty, Module.LoadedImageName); LOG(msglog);
			}
		}
		dwFrameNum++;
	} while (stFrame.AddrReturn.Offset);
	SymCleanup(hProcess);
	CloseHandle(hProcess);

	return TRUE;
}


LONG WINAPI OurCrashHandler(EXCEPTION_POINTERS* pExcept)
{
	const DWORD exceptionCode = pExcept->ExceptionRecord->ExceptionCode;
	//const PVOID exceptionAddress = pExcept->ExceptionRecord->ExceptionAddress;
	wchar_t* FaultTx;
	FILE* sgLogFile = nullptr;

	PROCESS_MEMORY_COUNTERS pmc;
	MEMORYSTATUS	MS;
	GlobalMemoryStatus(&MS);
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	SYSTEMTIME LocalTime;
	GetLocalTime(&LocalTime);

	SYSTEM_INFO info;
	GetSystemInfo(&info);

	if (exceptionCode & APPLICATION_ERROR_MASK) //Ingore custom SEH error codes, if any. (?)
	{
		return ExceptionContinueExecution;
	}
	if (exceptionCode >= 0x40010001 && exceptionCode <= 0x4001000F) //Ingore all DGB Exception Codes.
	{
		return ExceptionContinueExecution;
	}
	if (exceptionCode == 0xE06D7363 || exceptionCode == 0x406D1388) //C++ Threads.
	{
		return ExceptionContinueExecution;
	}
	if (exceptionCode >= 0x80004000 && exceptionCode <= 0x80004FFF)
	{
		return ExceptionContinueExecution;
	}
	if (exceptionCode == EXCEPTION_PRIV_INSTRUCTION)
	{
		return ExceptionContinueExecution;
	}
	if ((DWORD)pExcept->ExceptionRecord->ExceptionAddress >= 0x70000000)
	{
		//LOG(L"crashed on win32 kernals...maybe");
		return ExceptionContinueExecution;
	}

	else
	{

		DWORD EIP = pExcept->ContextRecord->Eip;
		DWORD ESP = pExcept->ContextRecord->Esp;
		DWORD EBP = pExcept->ContextRecord->Ebp;
		DWORD EBX = pExcept->ContextRecord->Ebx;
		DWORD EAX = pExcept->ContextRecord->Eax;
		DWORD ECX = pExcept->ContextRecord->Ecx;
		DWORD EDX = pExcept->ContextRecord->Edx;
		DWORD ESI = pExcept->ContextRecord->Esi;
		DWORD EDI = pExcept->ContextRecord->Edi;
		DWORD CurrentMem = pmc.WorkingSetSize / 1048576;
		DWORD TotalMem = MS.dwTotalPhys / 1048576;
		CHAR GetCompManu[128];
		LPCSTR getCompManu = "";

		wchar_t message[4096];
		wsprintfW(message, L"Dungeon Fighter - An error occurred during execution. The problem description for the occurring issue has been provided here.\nDNF.exe has crashed on: \nAddress : %X\nCode : %X\nWin32 : %X\nReason :%-24s\nCurrent Mem : %d\nTotal Mem: %d\nOperating System :%ws \n%s\nWhen: %d/%d/%d @ %02d:%02d:%02d.%d\nEAX=%08X  EBX=%08X  ECX=%08X\nEDX=%08X  EBP=%08X  ESI=%08X\nEDI=%08X  ESP=%08X  EIP=%08X\nSorry for the inconvenience. Please show this message to an administator.\nso that it may be referenced during bug resolution.\n\n\nA log file will also be written in the game's directory after this message closes, Please include the .log files to the Administrators as well. \nPress OK to return to the exit the application.\nYou will be logged off from the game.\n",
			pExcept->ExceptionRecord->ExceptionAddress,
			pExcept->ExceptionRecord->ExceptionCode,
			GetLastError(),
			FaultTx,
			CurrentMem, TotalMem,
			get_win_product_name().c_str(),
			LocalTime.wDay, LocalTime.wMonth, LocalTime.wYear,
			LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond, LocalTime.wMilliseconds,
			EAX, EBX, ECX, EDX, EBP, ESI, EDI, ESP, EIP);

		MessageBoxW(0, message, L"MS has crashed.", MB_OK);
		std::string fileName = (std::string(".\\LOGS\\CrashMS_") + ".log");
		errno_t errorCode = fopen_s(&sgLogFile, fileName.c_str(), "a+");
		LOG(message);
		ShowThreadStack(GetCurrentThread(), pExcept->ContextRecord, sgLogFile);
		fclose(sgLogFile);

		return EXCEPTION_CONTINUE_SEARCH;
	}


}
