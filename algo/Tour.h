#pragma once

#include <algo/Test.h>

namespace NAlgo {
    class Tour {
    public:
        Tour(Test test)
            : total_weight(LONG_LONG_MAX)
            , test(std::move(test))
        {
        }

        int64_t TotalWeight() {
            return total_weight;
        }

        void CalcTotalWeight()  {
            if (path.empty()) {
                total_weight = LONG_LONG_MAX;
            } else {
                total_weight = 0;
                for (size_t i = 0; i < path.size(); i++) {
                    total_weight += test.EvalDistance(path[i], path[(i + 1) % path.size()]);
                }
            }
        }

        std::string GetTestName()  {
            return test.GetTestName();
        }

        Test GetTest() {
            return test;
        }

        std::vector<int> path;
        std::vector<std::pair<int, int>> convergence;
    private:
        int64_t total_weight;
        Test test;
    };
}
