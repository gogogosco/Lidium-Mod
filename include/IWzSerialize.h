#pragma once
#include <IWzArchive.h>

class IWzSerialize
{
	virtual HRESULT get_persistentUOL(wchar_t**) = 0;
	virtual HRESULT raw_Serialize(IWzArchive*) = 0;
};

