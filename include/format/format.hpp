#pragma once

namespace mm_template::utils {
    class format final {
        public: static std::string va(const char * fmt, ...);
        static std::vector < std::string > split(const std::string & input,
            const std::string & delimiters);

        static void to_lower(std::string & input);
        static void to_upper(std::string & input);

        static std::wstring replace(const std::wstring & orig,
            const std::wstring & fnd,
                const std::wstring & repl);

        template < typename S >
        static auto stoi(const char * string, S
            default) -> S {
            char * end;

            const auto result = static_cast < S > (std::strtol(string, & end, 0));

            if ( * string == '\0' || * end != '\0') {
                return default;
            }

            return result;
        }
    };
}