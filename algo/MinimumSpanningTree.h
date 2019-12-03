#pragma once

#include <algo/ISolution.h>

namespace NAlgo {
    class MinimumSpanningTree : public ISolution {
    public:
        explicit MinimumSpanningTree(int version)
                : ISolution(version)
        {}

        Tour solve(const Test& test) override {
            int vertex = 0;
            Tour tour(test);

            return tour;
        }

        std::string solution_name() const override  {
            return "MinimumSpanningTree";
        }


    };
}



