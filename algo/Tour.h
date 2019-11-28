#pragma once

#include <algo/Test.h>

namespace NAlgo {
    class Tour {
    public:
        Tour(Test test)
            : total_weight(0)
            , test(std::move(test))
        {
        }

        int64_t TotalWeight() {
            return total_weight;
        }

        void CalcTotalWeight()  {
            total_weight = 0;
            for (size_t i = 0; i < path.size(); i++) {
                total_weight += test.EvalDistance(path[i], path[(i + 1) % path.size()]);
            }
        }

        std::vector<int> path;
    private:
        int64_t total_weight;
        Test test;
    };
}
