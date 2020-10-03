#ifndef EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
#define EXPERIMENTAL_SYSTEM_PROTOCOL_HPP

#include <array>
#include <cstdint>


namespace prtcl {

    using BODY_LENGTH_TYPE = size_t;

    constexpr size_t HEADER_BUF_LENGTH = sizeof(BODY_LENGTH_TYPE);


    using HeaderBufferType = std::array<char, HEADER_BUF_LENGTH>;

    class Header
    {
    private:
        HeaderBufferType itsBuffer ;

    public:
        Header() = default;
        ~Header() = default;

        HeaderBufferType & get_buffer()
        {
            return itsBuffer;
        }

        BODY_LENGTH_TYPE get_body_length() const
        {
            // Very naive. Only for one run machine and for prototyping :-)
            return *(static_cast<const size_t *>(static_cast<const void*>(itsBuffer.data())));
        }

        void set_body_length(BODY_LENGTH_TYPE len)
        {
            // Very naive. Only for one run machine and for prototyping :-)
            std::copy(static_cast<const char*>(static_cast<const void*>(&len)),
                      static_cast<const char*>(static_cast<const void*>(&len)) + sizeof(BODY_LENGTH_TYPE),
                      itsBuffer.data());
        }

    };


    using BodyBufferType = std::array<char, (8 << 17)>;  /* (8 << 17) 1048576 */

    class Body
    {
    private:
        BODY_LENGTH_TYPE itsFilledPartSize;

        std::shared_ptr<BodyBufferType> itsBuffer;

    public:
        Body()
                : itsFilledPartSize(0),
                  itsBuffer(std::make_shared<BodyBufferType>())
        {
        }

        void add_content(const std::string &content)
        {
            content.copy(itsBuffer->data() + itsFilledPartSize, content.length());
            itsFilledPartSize += content.length();
        }

        BodyBufferType & get_buffer()
        {
            return *itsBuffer;
        }

        std::string get_data() const
        {
            return std::string(itsBuffer->data(), itsFilledPartSize);
        }

        void set_as_empty(){
            itsFilledPartSize = 0;
        }

        void set_length(BODY_LENGTH_TYPE len)
        {
            itsFilledPartSize = len;
        }

        BODY_LENGTH_TYPE get_length()
        {
            return itsFilledPartSize;
        }


        ~Body() = default;
    };

}

#endif //EXPERIMENTAL_SYSTEM_PROTOCOL_HPP
