#include <main.hpp>

#include <utils/hook/hook.hpp>

#include <fstream>

#include <vector>

#include <algorithm>

#include <filesystem>

#include <shellapi.h>

#undef min
#undef max

//This WILL need adjusting for every executable
#define BINARY_PAYLOAD_SIZE 0x29000000
#define TLS_PAYLOAD_SIZE 0x60000

#pragma comment(linker, "/base:0x400000")
#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.main")
#pragma comment(linker, "/merge:.text=.main")
#pragma comment(linker, "/section:.main,re")

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];

#pragma data_seg(".main")
char main_data[0x1000] = {
    1
};

#pragma optimize("", off)
__declspec(thread) char tls_data[TLS_PAYLOAD_SIZE];
#pragma optimize("", on)

auto load_section(const HMODULE target,
    const HMODULE source, IMAGE_SECTION_HEADER * section) -> void {
    void * target_ptr = reinterpret_cast < void * > (reinterpret_cast < std::uint32_t > (target) + section -> VirtualAddress);
    const void * source_ptr = reinterpret_cast < void * > (reinterpret_cast < std::uint32_t > (source) + section -> PointerToRawData);

    if (section -> SizeOfRawData > 0) {
        const auto size_of_data = std::min(section -> SizeOfRawData, section -> Misc.VirtualSize);

        DWORD old_protect;
        VirtualProtect(target_ptr, size_of_data, PAGE_EXECUTE_READWRITE, & old_protect);

        std::memmove(target_ptr, source_ptr, size_of_data);
    }
}

auto load_sections(const HMODULE target,
    const HMODULE source) -> void {
    const auto dos_header = reinterpret_cast < IMAGE_DOS_HEADER * > (source);
    const auto nt_headers = reinterpret_cast < IMAGE_NT_HEADERS * > (reinterpret_cast < std::uint32_t > (source) + dos_header -> e_lfanew);

    auto section = IMAGE_FIRST_SECTION(nt_headers);

    for (auto i = 0 u; i < nt_headers -> FileHeader.NumberOfSections; ++i, ++section) {
        if (section) {
            load_section(target, source, section);
        }
    }
}

auto find_library(LPCSTR library) -> HMODULE {
    auto handle = GetModuleHandleA(library);

    if (!handle) {
        handle = LoadLibraryA(library);
    }

    return handle;
}

auto load_imports(const HMODULE target,
    const HMODULE source) -> void {
    const auto dos_header = reinterpret_cast < IMAGE_DOS_HEADER * > (source);
    const auto nt_headers = reinterpret_cast < IMAGE_NT_HEADERS * > (reinterpret_cast < std::uint32_t > (source) + dos_header -> e_lfanew);

    const auto import_directory = & nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    auto descriptor = PIMAGE_IMPORT_DESCRIPTOR(reinterpret_cast < std::uint32_t > (target) + import_directory -> VirtualAddress);

    while (descriptor -> Name) {
        std::string library_name = LPSTR(reinterpret_cast < std::uint32_t > (target) + descriptor -> Name);

        auto name_table_entry = reinterpret_cast < uintptr_t * > (reinterpret_cast < std::uint32_t > (target) + descriptor -> OriginalFirstThunk);
        auto address_table_entry = reinterpret_cast < uintptr_t * > (reinterpret_cast < std::uint32_t > (target) + descriptor -> FirstThunk);

        if (!descriptor -> OriginalFirstThunk) {
            name_table_entry = reinterpret_cast < uintptr_t * > (reinterpret_cast < std::uint32_t > (target) + descriptor -> FirstThunk);
        }

        while ( * name_table_entry) {
            FARPROC
            function = nullptr;

            if (IMAGE_SNAP_BY_ORDINAL( * name_table_entry)) {
                auto modu = find_library(library_name.data());
                if (modu) {
                    function = GetProcAddress(modu, MAKEINTRESOURCEA(IMAGE_ORDINAL( * name_table_entry)));
                }
            } else {
                auto
                import = PIMAGE_IMPORT_BY_NAME(reinterpret_cast < std::uint32_t > (target) + * name_table_entry);

                auto modu = find_library(library_name.data());
                if (modu) {
                    function = GetProcAddress(modu,
                        import -> Name);
                }
            }

            if (! function) {
                throw std::runtime_error("Unresolved import!");
            }

            * address_table_entry = reinterpret_cast < uintptr_t > (function);

            name_table_entry++;
            address_table_entry++;
        }

        descriptor++;
    }
}

auto verify_tls() -> void {
    const auto self = GetModuleHandleA(nullptr);
    const auto dos_header = reinterpret_cast < IMAGE_DOS_HEADER * > (self);
    const auto nt_headers = reinterpret_cast < IMAGE_NT_HEADERS * > (reinterpret_cast < std::uint32_t > (self) + dos_header -> e_lfanew);

    const auto self_tls = reinterpret_cast < PIMAGE_TLS_DIRECTORY > (reinterpret_cast < std::uint32_t > (self) + nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

    const auto ref = std::uintptr_t( & tls_data);
    const auto tls_index = * reinterpret_cast < std::uintptr_t * > (self_tls -> AddressOfIndex);

    const auto tls_vector = * reinterpret_cast < std::uintptr_t * > (__readfsdword(0x2C) + 4 * tls_index);

    const auto offset = ref - tls_vector;

    if (offset != 0 && offset != (sizeof(std::uintptr_t) * 2)) {
        throw std::runtime_error("TLS mapping is wrong!");
    }
}

auto manual_map(const char * bin_name) -> void {
    std::ifstream bin(bin_name, std::ifstream::binary);

    if (!bin.is_open()) {
        return;
    }

    bin.seekg(0, bin.end);
    auto binary_size = bin.tellg();
    bin.seekg(0, bin.beg);

    std::vector < std::uint8_t > executable_buffer;
    executable_buffer.resize(binary_size);

    bin.read(reinterpret_cast < char * > ( & executable_buffer[0]), binary_size);

    const auto modu = GetModuleHandleA(nullptr);
    const auto module_dos_header = reinterpret_cast < IMAGE_DOS_HEADER * > (modu);
    const auto module_nt_headers = reinterpret_cast < IMAGE_NT_HEADERS * > (reinterpret_cast < uint32_t > (modu) + module_dos_header -> e_lfanew);

    const auto source = reinterpret_cast < HMODULE > ( & executable_buffer[0]);
    const auto source_dos_header = reinterpret_cast < IMAGE_DOS_HEADER * > (source);
    const auto source_nt_headers = reinterpret_cast < IMAGE_NT_HEADERS * > (reinterpret_cast < uint32_t > (source) + source_dos_header -> e_lfanew);

    load_sections(modu, source);
    load_imports(modu, source);

    if (source_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size) {
        if (!module_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress) {
            __debugbreak();
        }

        const auto target_tls = reinterpret_cast < PIMAGE_TLS_DIRECTORY > (reinterpret_cast < uint32_t > (modu) + module_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
        const auto source_tls = reinterpret_cast < PIMAGE_TLS_DIRECTORY > (reinterpret_cast < uint32_t > (modu) + source_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

        const auto source_tls_size = source_tls -> EndAddressOfRawData - source_tls -> StartAddressOfRawData;
        const auto target_tls_size = target_tls -> EndAddressOfRawData - target_tls -> StartAddressOfRawData;

        const auto target_tls_index = * reinterpret_cast < DWORD * > (target_tls -> AddressOfIndex);
        const auto source_tls_index = * reinterpret_cast < DWORD * > (source_tls -> AddressOfIndex);
        * reinterpret_cast < DWORD * > (target_tls -> AddressOfIndex) += source_tls_index;

        DWORD old_protect;
        VirtualProtect(PVOID(target_tls -> StartAddressOfRawData), source_tls_size, PAGE_READWRITE, & old_protect);

        const auto tls_base = * reinterpret_cast < LPVOID * > (__readfsdword(0x2C) + (sizeof(std::uintptr_t) * source_tls_index) + (sizeof(std::uintptr_t) * target_tls_index));
        std::memmove(tls_base, PVOID(source_tls -> StartAddressOfRawData), source_tls_size);
        std::memmove(PVOID(target_tls -> StartAddressOfRawData), PVOID(source_tls -> StartAddressOfRawData), source_tls_size);
    }

    DWORD old_protect;
    VirtualProtect(module_nt_headers, 0x1000, PAGE_EXECUTE_READWRITE, & old_protect);

    module_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = source_nt_headers -> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    std::memmove(module_nt_headers, source_nt_headers, sizeof(IMAGE_NT_HEADERS) + (module_nt_headers -> FileHeader.NumberOfSections * (sizeof(IMAGE_SECTION_HEADER))));
}

int __cdecl main(int argc, char * argv[]) {
    FreeConsole();
    if (FindWindowA("MapleStoryClass", "MapleStory")) {
        exit(0);
    }
    if (std::filesystem::exists("MapleStory")) {
        SetDllDirectoryA("BINS\\");
        manual_map("MapleStory");
    } else {
        MessageBoxA(NULL, "ERROR: The file MapleStory is missing\nPlease download the MapleStory file and place it to the game directory\nYou will now be redirected to the downloads page", "MapleStory File not found", MB_OK);
        ShellExecuteA(NULL, "open", "http://website.com", NULL, NULL, NULL);
    }
    mm_template::main();
    mm_template::utils:: function < void() > (0x00A63FF3)(); //Entry point address of the application
}