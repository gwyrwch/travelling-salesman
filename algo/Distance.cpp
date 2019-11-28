#include "Distance.h"

#include <cmath>

namespace NAlgo {
    int64_t sqr(int64_t d) {
        return d  *  d;
    }

    std::function<int64_t(const Test&, int,int)> MakeDistanceFunction(const std::string& name) {
        if (name == "EUC_2D") {
            return [](const Test& test, int u, int v) {
                auto pt1 = test.GetPoint(u);
                auto pt2 = test.GetPoint(v);
                return int(std::sqrt(sqr(pt1.x - pt2.x) + sqr(pt1.y - pt2.y)) + 0.5);
            };
        } else {
            throw std::runtime_error("No such distance function + " + name);
        }
    }
}
