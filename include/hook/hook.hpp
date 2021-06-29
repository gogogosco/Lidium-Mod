#pragma once

namespace mm_template::utils {
    class unprotect {
        public:
            unprotect(std::uint32_t address, std::uint32_t size);
        ~unprotect();

        private:
            std::uint32_t address_;
        std::uint32_t size_;
        std::uint32_t protect_;
    };

    template < typename t > class
    function final {
        public: function (): function_(reinterpret_cast < t * > (nullptr)) {

        }

        function (t * ptr): function_(ptr) {

        }

        function (std::uint32_t address): function_(reinterpret_cast < t * > (address)) {

        }

        function (const utils:: function < t > & other): function_(other.function_) {

        }

        auto operator = (std::uint32_t address) -> utils:: function < t > & {
            this -> function_ = reinterpret_cast < t * > (address);

            return *this;
        }

        auto operator = (const utils:: function < t > & function) -> utils:: function < t > & {
            this -> function_ = function.function_;

            return *this;
        }

        template < typename...v > auto operator()(v && ...args) const -> decltype(auto) {
            return this -> function_(args...);
        }

        private: t * function_;
    };

    template < typename t > class vtable final {
        public: vtable(): table_(nullptr) {

        }

        vtable(std::uint32_t address): table_(reinterpret_cast < t * > (address)) {

        }

        vtable(const utils::vtable < t > & other): table_(other.table_) {

        }

        auto get() -> t * {
            return reinterpret_cast < t * > ( * reinterpret_cast < std::uint32_t ** > (this -> table_));
        }

        auto get(std::uint32_t index) -> std::uint32_t {
            return ( ** reinterpret_cast < std::uint32_t ** * > (this -> table_))[index];
        }

        template < typename f > auto call(std::uint32_t index) -> utils:: function < f > {
            return utils:: function < f > (( ** reinterpret_cast < std::uint32_t ** * > (this -> table_))[index]);
        }

        template < typename f > auto hook(std::uint32_t index, f
            function) -> std::uint32_t {
            const auto result = ** reinterpret_cast < std::uint32_t ** * > (this -> table_);
            const auto original = result[index];

            hook::set < std::uint32_t > (std::uint32_t( & result[index]), std::uint32_t(function));

            return original;
        }

        auto operator -> () -> t * {
            return reinterpret_cast < t * > ( * reinterpret_cast < std::uint32_t ** > (this -> table_));
        }

        private: t * table_;
    };

    namespace hook {
        enum class instr: std::uint8_t {
            nop = 0x90,
                mov = 0xB8,
                jmp = 0xE9,
                call = 0xE8,
                retn = 0xC3,
                retn_imm16 = 0xC2,
        };

        void retn(std::uint32_t address);
        void retn(std::uint32_t address, std::uint16_t size);

        void nop(std::uint32_t address, std::uint32_t size);

        void write(std::uint32_t address,
            const std::initializer_list < std::uint8_t > & bytes);
        void write_string(std::uint32_t address,
            const std::string & string);

        template < typename t > void set(std::uint32_t address, t value) {
            utils::unprotect $(address, sizeof(t));

            * reinterpret_cast < t * > (address) = value;
        }

        template < typename t > auto get(std::uint32_t address) {
            //utils::unprotect $(address, sizeof(t));

            return *reinterpret_cast < t * > (address);
        }

        template < typename t > void jump(std::uint32_t address, t
            function, std::uint32_t size) {
            utils::unprotect $(address, size);

            hook::set < hook::instr > (address, instr::jmp);
            hook::set < std::uint32_t > (address + 1, std::uint32_t(function) - address - 5);
        }

        template < typename t > void call(std::uint32_t address, t
            function) {
            utils::unprotect $(address, 5);

            hook::set < hook::instr > (address, instr::call);
            hook::set < std::uint32_t > (address + 1, std::uint32_t(function) - address - 5);
        }

        template < typename t > void return_value(std::uint32_t address, t value) {
            utils::unprotect $(address, 6);

            hook::set < hook::instr > (address, instr::mov);
            hook::set < std::uint32_t > (address + 1, std::uint32_t(value));
            hook::set < hook::instr > (address + 5, instr::retn);
        }

        template < typename t > auto detour(std::uint32_t source,
            const t & destination, std::uint32_t size) -> utils:: function < t > {
            const auto address = ::VirtualAlloc(nullptr, size + 5, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

            std::memcpy(address, reinterpret_cast < std::uint8_t * > (source), size + 5);

            hook::jump(source, destination, 5);
            hook::jump(reinterpret_cast < std::uint32_t > (address) + size, source + size, 5);

            return utils:: function < t > (reinterpret_cast < t * > (address));
        }
    }
}