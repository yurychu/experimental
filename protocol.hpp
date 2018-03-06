#ifndef EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
#define EXPERIMENTAL_SYSTEM_PROTOCOL_HPP

#include <array>
#include <cstdint>


namespace prtcl {

    class Header
    {
    public:
        enum { fixed_size = 4 };

    private:
        std::array<int8_t, fixed_size> itsData;

    public:
        template <std::size_t SIZE>
        explicit Header(std::array<int8_t, SIZE> & data)
                : itsData(data) {}

        ~Header() = default;

        size_t get_length() const { return itsData.size(); }
        void print_data() const { std::cout << atoi((char*)itsData.data()) << std::endl; }

    };

    class Body {

    };
}

#endif //EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
