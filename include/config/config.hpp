#pragma once

namespace mm_template {
    class config final {
        public: static void init();
        static std::vector < std::string > parse_char_array(char * unformatted_string, char * delimiter);

        static int CPUMaxPerformance;
    };
}