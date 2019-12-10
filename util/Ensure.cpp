
#include "Ensure.h"

namespace NUtil {
    void Ensure(bool correct, const std::string& if_false) {
        if (!correct)
            throw std::runtime_error(if_false);
    }
}