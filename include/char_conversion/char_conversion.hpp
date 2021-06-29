#pragma once
namespace adnf::utils {
    class char_conversion final {
        public: static wchar_t * widen_char(const char * contents);
        static char * normalize_wchar(const wchar_t * contents);

        static std::uint32_t get_codepage(std::uint32_t uiCharSet);
        static std::wstring ansi_to_unicode(const std::string & s, std::uint32_t uiCharSet);
        static std::string unicode_to_ansi(const std::wstring & ws, std::uint32_t uiCharSet);
        static std::wstring utf8_to_unicode(const std::string & s, std::uint32_t uiCharSet);
        static std::string unicode_to_utf8(const std::wstring & ws, std::uint32_t uiCharSet);
        static std::string utf8_to_ansi(const std::string & s, std::uint32_t uiCharSet);
        static wchar_t * ansi_to_unicode(const char * psz, std::uint32_t uiCharSet);
        static char * unicode_to_ansi(const wchar_t * pwsz, std::uint32_t uiCharSet);
        static wchar_t * utf8_to_unicode(const char * psz, std::uint32_t uiCharSet);
        static char * unicode_to_utf8(const wchar_t * pwsz, std::uint32_t uiCharSet);
        static char * utf8_to_ansi(const char * psz, std::uint32_t uiCharSet);

        static int string_to_byte_array(BYTE * bCode, char * Code);
    };
}