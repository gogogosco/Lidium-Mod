#include <stdafx.hpp>

#include <../hook/hook.hpp>

namespace mm_template::utils {
    unprotect::unprotect(std::uint32_t address, std::uint32_t size): address_(address), size_(size) {
        ::VirtualProtect(reinterpret_cast < void * > (address), size, PAGE_EXECUTE_READWRITE, reinterpret_cast < ::DWORD * > ( & this -> protect_));
    }

    unprotect::~unprotect() {
        ::VirtualProtect(reinterpret_cast < void * > (this -> address_), this -> size_, this -> protect_, reinterpret_cast < ::DWORD * > ( & this -> protect_));
    }

    void hook::retn(std::uint32_t address) {
        utils::hook::set < hook::instr > (address, instr::retn);
    }

    void hook::retn(std::uint32_t address, std::uint16_t size) {
        utils::hook::set < hook::instr > (address, instr::retn_imm16);
        utils::hook::set < std::uint16_t > (address + 1, size);
    }

    void hook::nop(std::uint32_t address, std::uint32_t size) {
        utils::unprotect $(address, size);

        std::memset(reinterpret_cast < void * > (address), 0x90, size);
    }

    void hook::write(std::uint32_t address,
        const std::initializer_list < std::uint8_t > & bytes) {
        utils::unprotect $(address, bytes.size());

        std::memcpy(reinterpret_cast < std::uint8_t * > (address), bytes.begin(), bytes.size());
    }

    void hook::write_string(std::uint32_t address,
        const std::string & string) {
        utils::unprotect $(address, string.size());

        std::strcpy(reinterpret_cast < char * > (address), & string[0]);
    }
}