#include "auxiliary.h"

#include <algorithm>

bool Is_Only_Characters(const std::string &string)
{
    return !std::any_of(string.begin(), string.end(), [](char ch) { return isspace(ch); });
}


