#include <cstdint>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include <boost/predef.h> // Tools to identify the OS.


#ifdef BOOST_OS_WINDOWS
# define _WIN32_WINNT 0x0501
# if _WIN32_WINNT <= 0x0502  // Windows Server 2003 or earlier.
#  define BOOST_ASIO_DISABLE_IOCP
#  define BOOST_ASIO_ENABLE_CANCELIO
# endif
#endif


using namespace boost;


namespace http_errors {
    enum http_error_codes {
        invalid_response = 1
    };


    class http_errors_category : public boost::system::error_category
    {
    public:
        const char * name() const BOOST_SYSTEM_NOEXCEPT override {
            return "http_errors";
        }

        std::string message(int e) const override {
            switch (e) {
                case invalid_response : {
                    return "Server response cannot be parsed.";
                }
                default : {
                    return "Unknown error.";
                }
            }
        }

        const boost::system::error_category &
        get_http_errors_category(){
            static http_errors_category cat;
            return cat;
        }

        boost::system::error_code
        make_error_code(http_error_codes e){
            return { static_cast<int>(e), get_http_errors_category() };
        }

    };
}  // namespace http_errors


// add to boost ues our err codes http
namespace boost {
    namespace system {

        template<>
        struct is_error_code_enum
        <http_errors::http_error_codes>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

    }
}


int main()
{


    return 0;
};
