#pragma once
#include "pch.h"
#include <OAIdl.h>

//we should implement it correctly with the proper inheritance (inherit IUnknown and IWzSerialize)
//but this should have the correct offsets
//credits to shavit
class IWzProperty
{
public:
	// IUnknown
	virtual HRESULT QueryInterface(const IID* riid, void** ppvObject) = 0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;

	// IWzSerialize
	virtual HRESULT get_persistentUOL(wchar_t**) = 0;
	virtual HRESULT raw_Serialize(IWzArchive*) = 0;

	// IWzProperty
	virtual HRESULT get_item(wchar_t*, tagVARIANT*) = 0;
	virtual HRESULT put_item(wchar_t*, tagVARIANT) = 0;
	virtual HRESULT get__NewEnum(IUnknown**) = 0;
	virtual HRESULT get_count(unsigned int*) = 0;
	virtual HRESULT raw_Add(wchar_t*, tagVARIANT, tagVARIANT) = 0;
	virtual HRESULT raw_Remove(wchar_t*) = 0;
	virtual HRESULT raw_Import(wchar_t*) = 0;
};