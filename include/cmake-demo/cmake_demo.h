#ifndef __CMAKE_DEMO_H__
#define __CMAKE_DEMO_H__

#include <string>

#include "reverse.h"
#include "palindrome.h"

class CmakeDemo {
public:
    CmakeDemo();
    ~CmakeDemo();

    bool is_palindrome(const std::string& toCheck);
    std::string reverse(std::string& toReverse);

private:
    Reverse _reverse;
    Palindrome _palindrome;
};

#endif //__CMAKE_DEMO_H__