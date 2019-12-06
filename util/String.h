#pragma once

#include <vector>
#include <string>

namespace NUtil {
    std::vector<std::string> Split(const std::string& s, char split_char);
    std::vector<std::string> SplitAndTrim(const std::string& s, char split_char);
    std::string Trim(std::string s);
}

