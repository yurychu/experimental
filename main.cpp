#include <iostream>
#include <string>
#include <array>



int main()
{
//
//    int x;
//    x = 100500;
//
//    auto some = static_cast<char*>(static_cast<void*>(&x));


    size_t x = 10050070080;
    char bytes [sizeof(size_t)];

    std::copy(static_cast<const char*>(static_cast<const void*>(&x)),
              static_cast<const char*>(static_cast<const void*>(&x)) + sizeof(x),
              bytes);


    size_t some = *(static_cast<size_t*>(static_cast<void *>(bytes)));

    return 0;
}
