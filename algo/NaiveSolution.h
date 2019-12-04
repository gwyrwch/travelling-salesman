#pragma once

#include <algo/ISolution.h>
#include <algorithm>


namespace NAlgo {
    class NaiveSolution : public ISolution {
    public:
        explicit NaiveSolution(int version)
                : ISolution(version)
        {}

        Tour solve(const Test& test) override {
            int vertex = 0;
            Tour tour(test);
            int64_t answer = LONG_LONG_MAX;

            std::vector<int> v;
            for(int i = 0; i < test.GetVertexNum(); i++) {
                v.push_back(i);
            }


            do {
                Tour candidate(test);
                candidate.path = v;
                candidate.CalcTotalWeight();

                if (candidate.TotalWeight() < answer) {
                    tour = candidate;
                    answer = candidate.TotalWeight();
                }
            } while(std::next_permutation(v.begin(), v.end()));

            return tour;
        }

        std::string solution_name() const override  {
            return "NaiveSolution";
        }
    };
}




