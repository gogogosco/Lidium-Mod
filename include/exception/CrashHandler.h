#pragma once
#include <Shlwapi.h>
#include <ImageHlp.h>
#include <PsApi.h>

#pragma comment(lib, "DbgHelp.lib")

#define LOG(...) fwprintf(sgLogFile, __VA_ARGS__)

	
LONG WINAPI MapleCrashHandler(EXCEPTION_POINTERS* pExcept)
{
	const DWORD exceptionCode = pExcept->ExceptionRecord->ExceptionCode;
	//const PVOID exceptionAddress = pExcept->ExceptionRecord->ExceptionAddress;
	wchar_t* FaultTx;

	PROCESS_MEMORY_COUNTERS pmc;
	MEMORYSTATUS	MS;
	GlobalMemoryStatus(&MS);
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	SYSTEMTIME LocalTime;
	GetLocalTime(&LocalTime);

	SYSTEM_INFO info;
	GetSystemInfo(&info);

	//if (pExcept->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	//return EXCEPTION_CONTINUE_SEARCH;

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

		wchar_t message[4096];

		MessageBoxW(0, message, L"MapleStory has crashed.", MB_OK);

		return EXCEPTION_CONTINUE_SEARCH;
	}


}
