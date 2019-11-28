#pragma once

#include <algo/Test.h>

#include <functional>
#include <string>

namespace NAlgo {
    std::function<int64_t(const Test&, int,int)> MakeDistanceFunction(const std::string& name);
}
