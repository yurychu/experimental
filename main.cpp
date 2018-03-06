#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "protocol.hpp"


int main() {
    std::array<int8_t, prtcl::Header::fixed_size> input_data = {'0', '0', '4', '3'};
    prtcl::Header header_cust(input_data);
    {
        using namespace std; // For sprintf and memcpy.
        char header[4 + 1] = "";
        sprintf(header, "%4d", 46);
        std::cout << header << std::endl;

        std::cout << atoi(header) << std::endl;
    }

    std::cout << header_cust.get_length() << std::endl;
    header_cust.print_data();
    return 0;
}
