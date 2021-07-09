#pragma once
#include <Shlwapi.h>
#include <ImageHlp.h>
#include <PsApi.h>

#pragma comment(lib, "DbgHelp.lib")
	
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

LONG WINAPI MapleCrashHandler(EXCEPTION_POINTERS* pExcept)
{
	const DWORD exceptionCode = pExcept->ExceptionRecord->ExceptionCode;
	//const PVOID exceptionAddress = pExcept->ExceptionRecord->ExceptionAddress;
	std::string FaultTx;

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
		switch (exceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION: FaultTx = "ACCESS VIOLATION";		break;
		case EXCEPTION_DATATYPE_MISALIGNMENT: FaultTx = "DATATYPE MISALIGNMENT"; break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: FaultTx = "FLT DIVIDE BY ZERO";	break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: FaultTx = "ARRAY BOUNDS EXCEEDED";	break;
		case EXCEPTION_FLT_DENORMAL_OPERAND: FaultTx = "FLT DENORMAL OPERAND";	break;
		case EXCEPTION_FLT_INEXACT_RESULT: FaultTx = "FLT INEXACT RESULT";	break;
		case EXCEPTION_FLT_INVALID_OPERATION: FaultTx = "FLT INVALID OPERATION";	break;
		case EXCEPTION_FLT_OVERFLOW: FaultTx = "FLT OVERFLOW";			break;
		case EXCEPTION_FLT_STACK_CHECK: FaultTx = "FLT STACK CHECK";		break;
		case EXCEPTION_FLT_UNDERFLOW: FaultTx = "FLT UNDERFLOW";			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION: FaultTx = "ILLEGAL INSTRUCTION";	break;
		case EXCEPTION_IN_PAGE_ERROR: FaultTx = "IN PAGE ERROR";			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO: FaultTx = "INT DEVIDE BY ZERO";	break;
		case EXCEPTION_INT_OVERFLOW: FaultTx = "INT OVERFLOW";			break;
		case EXCEPTION_INVALID_DISPOSITION: FaultTx = "INVALID DISPOSITION";	break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:FaultTx = "NONCONTINUABLE EXCEPTION"; break;
		case EXCEPTION_PRIV_INSTRUCTION: FaultTx = "PRIVILEGED INSTRUCTION"; break;
		case EXCEPTION_SINGLE_STEP: FaultTx = "SINGLE STEP";			break;
		case EXCEPTION_STACK_OVERFLOW: FaultTx = "STACK OVERFLOW";		break;
		case DBG_CONTROL_C:  FaultTx = "CONTROL C";		break;
			//case DBG_PRINTEXCEPTION_C:
			//case 0x4001000AL: //DBG_PRINTEXCEPTION_WIDE_C
			//case 0x406D1388: //Thread naming
			//case 0xE06D7363: //C++ Exceptions
		default: FaultTx = "UNKNOWN";
		}

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

		wchar_t message[4096];

		std::wstring stemp = s2ws(FaultTx);
		LPCWSTR result = stemp.c_str();

		wsprintfW(message, L"An error occurred during execution. The problem description for the occurring issue has been provided here.\nThe game has crashed on: \nAddress : %X\nCode : %X\nWin32 : %X\nCurrent Mem : %d\nTotal Mem: %d\nWhen: %d/%d/%d @ %02d:%02d:%02d.%d\nEAX=%08X  EBX=%08X  ECX=%08X\nEDX=%08X  EBP=%08X  ESI=%08X\nEDI=%08X  ESP=%08X  EIP=%08X\nSorry for the inconvenience. Please show this message to an administator so that it may be referenced during bug resolution.\n\nPress OK to return to the exit the application.\nYou will be logged off from the game.\n",
			pExcept->ExceptionRecord->ExceptionAddress,
			pExcept->ExceptionRecord->ExceptionCode,
			GetLastError(),
			CurrentMem, TotalMem,
			LocalTime.wDay, LocalTime.wMonth, LocalTime.wYear,
			LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond, LocalTime.wMilliseconds,
			EAX, EBX, ECX, EDX, EBP, ESI, EDI, ESP, EIP);


		MessageBoxW(0, message, result, MB_OK);

		return EXCEPTION_CONTINUE_SEARCH;
	}


}
