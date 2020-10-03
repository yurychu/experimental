#include <iostream>
#include <string>
#include <array>
#include <limits>
#include <cstddef>

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <opencv2/opencv.hpp>


namespace fs = boost::filesystem;

int main()
{
    wchar_t a;

//    cv::Mat img = cv::Mat::ones(cv::Size(200, 400), CV_8UC3);
//    cv::imwrite("my_file.jpg", img);
//    boost::system::error_code ec;
//
//    fs::path p("some/make_thisi/");
//    try
//    {
////        auto result = fs::create_directories(p);
////        std::cout << "Res create dirs: " << result << std::endl;
//
//        auto exist = fs::is_directory(p, ec);
//        std::cout << "Res is dir: " << exist << std::endl;
//
//        if (ec.value() != 0){
//            std::cout << "Err is_directory code: " << ec.value() <<  " msg: " << ec.message() << std::endl;
//        }
//        else {
//            std::cout << "Save in existing: " << std::endl;
//        }
//
//    }
//    catch (fs::filesystem_error &e)
//    {
//        std::cerr << e.what() << '\n';
//    }

//    u_char my = 4;
//
//    std::string some = "abc";
//    std::string res = some + std::to_string(my);
//
//    std::cout << res << std::endl;

    std::cout << "size_t: " << std::dec << std::numeric_limits<std::size_t>::max() << std::endl;
    auto param = 17457983742491709948UL;

    std::cout << sizeof(param) << std::endl;

//    const std::string filename = "vspeed.bin";
//    std::ifstream ifile(filename);
//    if (!ifile){
//        std::cerr << "Error opening file." << std::endl;
//        exit(EXIT_SUCCESS);
//    }
//    std::stringstream buffer;
//    buffer << ifile.rdbuf();
//
//    std::string str = buffer.str();
//    std::cout << "Content file: " << str << std::endl;
//    size_t res_hash_value = boost::hash_value(str);
//    std::cout << res_hash_value << std::endl;
//
//    Content file: helloa
//    3005401605562312250

    return 0;
}
