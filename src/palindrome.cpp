#include "palindrome.h"

bool Palindrome::is_palindrome(const std::string& toCheck)
{

    if (toCheck == std::string(toCheck.rbegin(), toCheck.rend())) {
        return true;
    }

    return false;
}
