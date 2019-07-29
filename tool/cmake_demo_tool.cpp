#include <iostream>
#include <string>

#include "cmake_demo.h"

int main(int argc, char const *argv[]) {
    std::string str;
    CmakeDemo cmake_demo;
    while(std::cin >> str) {
        std::string res = cmake_demo.reverse(str);
        std::cout << "reverse: " << res << std::endl;
        std::cout << "is_palindrome: " << cmake_demo.is_palindrome(str) << std::endl;
    }
    return 0;
}
