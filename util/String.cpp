#include "String.h"

namespace NUtil {
    std::vector<std::string> Split(const std::string &s, char split_char) {
        std::vector<std::string> tokens;
        std::string token;

        for_each(s.begin(), s.end(), [&](char c) {
            if (c != split_char)
                token += c;
            else {
                if (token.length()) tokens.push_back(token);
                token.clear();
            }
        });
        if (token.length()) tokens.push_back(token);

        return tokens;
    }

    std::string Trim(std::string s) {
        for (int i = 0; i < 2; i++) {
            while (!s.empty() && (isspace(s.back()) || s.back() == '\t')) {
                s.pop_back();
            }
            std::reverse(s.begin(), s.end());
        }
        return s;
    }

    std::vector<std::string> SplitAndTrim(const std::string &s, char split_char) {
        auto tokens = Split(s, split_char);
        for (auto& t : tokens) {
            t = Trim(t);
        }
        return tokens;
    }
}
