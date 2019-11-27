#pragma once

#include <algo/ISolution.h>

namespace NAlgo {
    class NearestNeighbour : public ISolution {
    public:
        explicit NearestNeighbour(int version)
            : ISolution(version)
        {}

        Tour solve(Test test) override {
            // TODO
            return Tour(test);
        }

        std::string solution_name() const override  {
            return "NearestNeighbour";
        }
    };
}