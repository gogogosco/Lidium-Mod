#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "MapleHooks.h"
#include <cstdint>
#include <stdio.h>

// define your hook functions

namespace MapleHooks
{

	void __cdecl ExampleCDecl_Hook(void* pArg1, int nArg2)
	{
		// do stuff
	}

	int __fastcall ExampleFunc_thiscall2(void* pThis, void* edx, int nArg1)
	{
		// do stuff
		return 0;
	}


}