cmake_minimum_required(VERSION 3.9)
project(experimental_system)

set(CMAKE_CXX_STANDARD 14)


link_directories(
#        /home/yurychu/Developing/installed_libs/boost166/lib
        /home/yurych4/developing/builds/builded_libs/lib
#        /home/yurychu/Developing/installed_libs/ocv320pure/lib
        /home/yurych4/developing/builds/builded_libs/opencv_native_lib320/lib
)

include_directories(
#        /home/yurychu/Developing/installed_libs/boost166/include
        /home/yurych4/developing/builds/builded_libs/include
#        /home/yurychu/Developing/installed_libs/ocv320pure/include
        /home/yurych4/developing/builds/builded_libs/opencv_native_lib320/include
)


add_executable(experimental_system main.cpp)
add_executable(
        stream_server
        stream_server.cpp
        protocol.hpp
)

add_executable(
        cook_code
        cook_code.cpp
)

add_executable(
        stream_client
        stream_client.cpp
        protocol.hpp
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
        cook_code
        boost_system
        pthread
)