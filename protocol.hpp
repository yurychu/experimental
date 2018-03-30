#ifndef EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
#define EXPERIMENTAL_SYSTEM_PROTOCOL_HPP

#include <array>
#include <cstdint>


namespace prtcl {

    constexpr u_char START_WORD_LEN_BYTES = 8;
    constexpr u_char META_DATA_LEN_BYTES = 8;
    constexpr u_char DATA_LEN_BYTES = 8;
    constexpr u_char SUM_LENGTH_BYTES = START_WORD_LEN_BYTES + META_DATA_LEN_BYTES + DATA_LEN_BYTES;

    using HeaderBufferType = std::array<char, SUM_LENGTH_BYTES>;

    class Header
    {
    private:
        HeaderBufferType itsBuffer;

    public:
        Header() = default;
        ~Header() = default;

        HeaderBufferType & get_buffer()
        {
            return itsBuffer;
        }

        size_t get_metadata_size() const
        {
            std::string res(itsBuffer.data() + START_WORD_LEN_BYTES, META_DATA_LEN_BYTES);
            return std::stoul(res);
        }

        size_t get_data_size() const
        {
            std::string res(itsBuffer.data() + START_WORD_LEN_BYTES + META_DATA_LEN_BYTES, DATA_LEN_BYTES);
            return std::stoul(res);
        }

    };


    using BodyBufferType = std::array<char, (8 << 17)>;  /* (8 << 17) 1048576 */

    class Body
    {
    private:
        BodyBufferType itsBuffer;
        size_t itsMetadataSize;
        size_t itsDataSize;

    public:
        Body()
                : itsBuffer(),
                  itsMetadataSize(0),
                  itsDataSize(0)
        {
        }

        explicit Body(const Header & header)
                : itsBuffer(),
                  itsMetadataSize(header.get_metadata_size()),
                  itsDataSize(header.get_data_size())
        {
        }

        void set_header(const Header & header)
        {
            itsMetadataSize = header.get_metadata_size();
            itsDataSize = header.get_data_size();
        }

        size_t get_sum_size() const
        {
            return itsMetadataSize + itsDataSize;
        }

        BodyBufferType & get_buffer()
        {
            return itsBuffer;
        }

        std::string get_metadata() const
        {
            return std::string(itsBuffer.data(), itsMetadataSize);
        }

        std::string get_data() const
        {
            return std::string(itsBuffer.data() + itsMetadataSize, itsDataSize);
        }

        ~Body() = default;
    };

}

#endif //EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
