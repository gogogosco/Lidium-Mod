#include <../config/config.hpp>

#include <../format/format.hpp>

#include <../console/console.hpp>

#include <filesystem>

#pragma warning(push, 0) //Removed deprecation warnings
#include <../INI/IniReader.h>

#pragma warning(pop)

namespace mm_template {
    int config::CPUMaxPerformance;
    void config::init() {
        CIniReader iniReader("settings.ini");

        if (!std::filesystem::exists("settings.ini")) {
            iniReader.WriteInteger("Settings", "CPUMaxPerformance", 0);
        }

        CPUMaxPerformance = iniReader.ReadInteger("Other", "CPUMaxPerformance", 0);
    }
}