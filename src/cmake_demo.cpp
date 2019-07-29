#include "cmake_demo.h"

CmakeDemo::CmakeDemo():_reverse(), _palindrome() {
}

CmakeDemo::~CmakeDemo() {
}

bool CmakeDemo::is_palindrome(const std::string& str) {
    return _palindrome.is_palindrome(str);
}

std::string CmakeDemo::reverse(std::string& str) {
    return _reverse.reverse(str);
}