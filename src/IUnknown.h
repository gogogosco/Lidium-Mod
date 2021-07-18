#pragma once
class IUnknown
{
public:
	virtual HRESULT QueryInterface(const IID* riid, void** ppvObject) = 0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
};

