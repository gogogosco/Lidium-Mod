#pragma once
#include "pch.h"
#include <OAIdl.h>
#include <IWzArchive.h>
#include <IUnknown.h>
#include <IWzSerialize.h>

//we should implement it correctly with the proper inheritance (inherit IUnknown and IWzSerialize)
//but this should have the correct offsets
//credits to shavit
class IWzProperty
{
public:
	virtual HRESULT get_item(wchar_t*, tagVARIANT*) = 0;
	virtual HRESULT put_item(wchar_t*, tagVARIANT) = 0;
	virtual HRESULT get__NewEnum(IUnknown**) = 0;
	virtual HRESULT get_count(unsigned int*) = 0;
	virtual HRESULT raw_Add(wchar_t*, tagVARIANT, tagVARIANT) = 0;
	virtual HRESULT raw_Remove(wchar_t*) = 0;
	virtual HRESULT raw_Import(wchar_t*) = 0;
};