cmake_minimum_required(VERSION 3.9)
project(experimental_system)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)


link_directories(
        /home/yurychu/Developing/installed_libs/boost166/lib
#        /home/yurych4/developing/builds/builded_libs/lib
        /home/yurychu/Developing/installed_libs/ocv320pure/lib
#        /home/yurych4/developing/builds/builded_libs/opencv_native_lib320/lib
)

include_directories(
        /home/yurychu/Developing/installed_libs/boost166/include
#        /home/yurych4/developing/builds/builded_libs/include
        /home/yurychu/Developing/installed_libs/ocv320pure/include
#        /home/yurych4/developing/builds/builded_libs/opencv_native_lib320/include
)


add_executable(
        main
        main.cpp
)

target_link_libraries(
        main
        boost_filesystem
        boost_system
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
)

add_executable(
        stream_server
        stream_server.cpp
        protocol.hpp
)

add_executable(
        writing_to_socket
        writing_to_socket.cpp
)
add_executable(
        reading_from_socket
        reading_from_socket.cpp
)

add_executable(
        stream_client
        stream_client.cpp
        protocol.hpp
)

add_executable(
        other_manipulation
        other_manipulation.cpp
)

add_executable(
        cook_client
        cook_client.cpp
)

add_executable(
        cook_server
        cook_server.cpp
)

add_executable(
        cook_server_parallel
        cook_server_parallel.cpp
)

add_executable(
        cook_server_async
        cook_server_async.cpp
)

add_executable(
        cook_http_client
        cook_http_client.cpp
)

add_executable(
        module_one
        module_one.cpp
        protocol.hpp
        FunctionalServer.hpp
        FunctionalClient.hpp
        TaskFunctional.hpp
)

target_link_libraries(
        stream_server
        boost_system
        pthread
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
)

target_link_libraries(
        stream_client
        boost_system
        pthread
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
)

target_link_libraries(
        writing_to_socket
        boost_system
        pthread
)

target_link_libraries(
        reading_from_socket
        boost_system
        pthread
)

target_link_libraries(
        other_manipulation
        boost_system
        pthread
)

target_link_libraries(
        cook_client
        boost_system
        pthread
)

target_link_libraries(
        cook_server
        boost_system
        boost_thread
        pthread
)

target_link_libraries(
        cook_server_parallel
        boost_system
        boost_thread
        pthread
)

target_link_libraries(
        cook_server_async
        boost_system
        boost_thread
        pthread
)

target_link_libraries(
        cook_http_client
        boost_system
        boost_thread
        pthread
)

target_link_libraries(
        module_one
        boost_system
        boost_thread
        pthread
)