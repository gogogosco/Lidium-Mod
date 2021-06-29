#include <../char_conversion/char_conversion.hpp>

const char HexArray[] = "0123456789ABCDEF0123456789abcdef";

namespace adnf::utils
{
	wchar_t *char_conversion::widen_char(const char *contents)
	{
		const size_t c_len = strlen(contents) + 1;
		wchar_t *wretn = new wchar_t[c_len];
		mbstowcs(wretn, contents, c_len);
		return wretn;
	}

	char *char_conversion::normalize_wchar(const wchar_t *contents)
	{
		const size_t wc_len = wcslen(contents) + 1;
		char *cretn = new char[wc_len];
		wcstombs(cretn, contents, wc_len);
		return cretn;
	}

	std::uint32_t char_conversion::get_codepage(std::uint32_t uiCharSet)
	{
		switch (uiCharSet)
		{
		case SHIFTJIS_CHARSET:
			return 932;
		case HANGUL_CHARSET:
			return 949;
		case GB2312_CHARSET:
			return 936;
		case CHINESEBIG5_CHARSET:
			return 950;
		case GREEK_CHARSET:
			return 1253;
		case TURKISH_CHARSET:
			return 1254;
		case HEBREW_CHARSET:
			return 1255;
		case ARABIC_CHARSET:
			return 1256;
		case BALTIC_CHARSET:
			return 1257;
		case THAI_CHARSET:
			return 874;
		case EASTEUROPE_CHARSET:
			return 1250;
		default:
			return 1252;
		}
	}

	std::wstring char_conversion::ansi_to_unicode(const std::string &s, std::uint32_t uiCharSet)
	{
		int iLen = (int)s.length();
		if (iLen <= 0)
			return L"";

		int iUnicodeLen = MultiByteToWideChar(char_conversion::get_codepage(uiCharSet), 0, s.c_str(), -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[iUnicodeLen + 1];
		ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

		MultiByteToWideChar(char_conversion::get_codepage(uiCharSet), 0, s.c_str(), -1, (LPWSTR)pUnicode, iUnicodeLen);
		std::wstring ws(pUnicode);
		delete[] pUnicode;

		return ws;
	}

	std::string char_conversion::unicode_to_ansi(const std::wstring &ws, std::uint32_t uiCharSet)
	{
		int iANSILen = WideCharToMultiByte(char_conversion::get_codepage(uiCharSet), 0, ws.c_str(), -1, NULL, 0, NULL, FALSE);
		if (iANSILen <= 0)
			return "";

		char *psz = new char[iANSILen + 1];
		ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

		WideCharToMultiByte(char_conversion::get_codepage(uiCharSet), 0, ws.c_str(), -1, psz, iANSILen, NULL, FALSE);
		std::string s(psz);
		delete[] psz;

		return s;
	}

	std::wstring char_conversion::utf8_to_unicode(const std::string &s, std::uint32_t uiCharSet)
	{
		int iLen = (int)s.length();
		if (iLen <= 0)
			return L"";

		int iUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[iUnicodeLen + 1];
		ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, (LPWSTR)pUnicode, iUnicodeLen);
		std::wstring ws(pUnicode);
		delete[] pUnicode;

		return ws;
	}

	std::string char_conversion::unicode_to_utf8(const std::wstring &ws, std::uint32_t uiCharSet)
	{
		int iANSILen = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, NULL, 0, NULL, FALSE);
		if (iANSILen <= 0)
			return "";

		char *psz = new char[iANSILen + 1];
		ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

		WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, psz, iANSILen, NULL, FALSE);
		std::string s(psz);
		delete[] psz;

		return s;
	}

	std::string char_conversion::utf8_to_ansi(const std::string &s, std::uint32_t uiCharSet)
	{
		std::wstring ws = utf8_to_unicode(s, uiCharSet);
		return char_conversion::unicode_to_ansi(ws, uiCharSet);
	}

	wchar_t *char_conversion::ansi_to_unicode(const char *psz, std::uint32_t uiCharSet)
	{
		int iLen = (int)strlen(psz);
		if (iLen <= 0)
			return NULL;

		int iUnicodeLen = MultiByteToWideChar(char_conversion::get_codepage(uiCharSet), 0, psz, -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[iUnicodeLen + 1];
		ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

		MultiByteToWideChar(char_conversion::get_codepage(uiCharSet), 0, psz, -1, (LPWSTR)pUnicode, iUnicodeLen);
		return pUnicode;
	}

	char *char_conversion::unicode_to_ansi(const wchar_t *pwsz, std::uint32_t uiCharSet)
	{
		int iANSILen = WideCharToMultiByte(char_conversion::get_codepage(uiCharSet), 0, pwsz, -1, NULL, 0, NULL, FALSE);
		if (iANSILen <= 0)
			return NULL;

		char *psz = new char[iANSILen + 1];
		ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

		WideCharToMultiByte(char_conversion::get_codepage(uiCharSet), 0, pwsz, -1, psz, iANSILen, NULL, FALSE);
		return psz;
	}

	wchar_t *char_conversion::utf8_to_unicode(const char *psz, std::uint32_t uiCharSet)
	{
		int iLen = (int)strlen(psz);
		if (iLen <= 0)
			return NULL;

		int iUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, psz, -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[iUnicodeLen + 1];
		ZeroMemory(pUnicode, (iUnicodeLen + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_UTF8, 0, psz, -1, (LPWSTR)pUnicode, iUnicodeLen);

		return pUnicode;
	}

	char *char_conversion::unicode_to_utf8(const wchar_t *pwsz, std::uint32_t uiCharSet)
	{
		int iANSILen = WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, NULL, 0, NULL, FALSE);
		if (iANSILen <= 0)
			return NULL;

		char *psz = new char[iANSILen + 1];
		ZeroMemory(psz, (iANSILen + 1) * sizeof(char));

		WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, psz, iANSILen, NULL, FALSE);

		return psz;
	}

	char *char_conversion::utf8_to_ansi(const char *psz, std::uint32_t uiCharSet)
	{
		if ((int)strlen(psz) <= 0)
			return NULL;

		wchar_t *pwsz = utf8_to_unicode(psz, uiCharSet);
		char *pszOut = unicode_to_ansi(pwsz, uiCharSet);
		delete[] pwsz;
		return pszOut;
	}

	int char_conversion::string_to_byte_array(BYTE *bCode, char *Code)
	{
		int iSize = 0, i, j;
		for (i = 0; Code[i]; i++)
		{
			for (j = 0; HexArray[j]; j++)
			{
				if (Code[i] == HexArray[j])
				{
					if (iSize % 2 == 0)
					{
						bCode[iSize / 2] = (j % 0x10) * 0x10;
					}
					else
					{
						bCode[iSize / 2] += j % 0x10;
					}
					iSize++;
					break;
				}
			}
		}
		if (iSize % 2)
		{
			return 0;
		}
		return (iSize / 2);
	}
}