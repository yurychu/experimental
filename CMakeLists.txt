cmake_minimum_required(VERSION 3.15)
project(experim_some_code_proj)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.72.0 COMPONENTS program_options REQUIRED)


add_executable(
        client
        code_parts/client.cpp
        code_parts/src/client/RunClient.hpp
        code_parts/src/client/RunClient.cpp
)

target_compile_definitions(
        client
        PRIVATE
        #        BOOST_LOG_ENABLE_WARNINGS
        #        BOOST_THREAD_DYN_LINK
        #        BOOST_LOG_DYN_LINK
        BOOST_ALL_DYN_LINK
)

if (WIN32)
    target_compile_definitions(
            client
            PRIVATE
            _WIN32_WINNT=0x0A00  # windows 10 only
    )
endif()

target_include_directories(
        client
        PRIVATE
        code_parts/src/client
)

target_link_directories(
        client
        PRIVATE
        ${Boost_LIBRARY_DIRS}
)

target_link_libraries(
        client
        PRIVATE
        Boost::program_options
)
