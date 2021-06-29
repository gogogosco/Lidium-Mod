#include <../format/format.hpp>

namespace mm_template::utils {
    std::string format::va(const char * fmt, ...) {
        auto va = ::va_list();

        va_start(va, fmt);

        char result[512] {};

        std::vsprintf(result, fmt, va);

        return std::string(result);
    }

    std::vector < std::string > format::split(const std::string & input,
        const std::string & delimiters) {
        auto result = std::vector < std::string > ();

        boost::split(result, input, boost::is_any_of(delimiters));

        return result;
    }
}