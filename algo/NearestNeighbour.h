#pragma once

#include <algo/ISolution.h>

namespace NAlgo {
    class NearestNeighbour : public ISolution {
    public:
        explicit NearestNeighbour(int version, SolutionConfig config)
            : ISolution(version, std::move(config))
        {}

        Tour solve(const Test& test) override {
            int vertex = 0;
            Tour tour(test);

            std::vector<char> used(test.GetVertexNum(), false);
            tour.path.push_back(vertex);
            used[vertex] = true;

            for (int i = 0; i < test.GetVertexNum() - 1; i++) {
                int opt_next_vertex = -1;
                for (int j = 0; j < test.GetVertexNum(); j++) {
                    if (!used[j] && (opt_next_vertex == -1 || test.EvalDistance(vertex, j) < test.EvalDistance(vertex, opt_next_vertex)))  {
                        opt_next_vertex = j;
                    }
                }
                vertex = opt_next_vertex;
                used[vertex] = true;
                tour.path.push_back(vertex);
            }

            tour.CalcTotalWeight();
            return tour;
        }

        std::string solution_name() const override  {
            return "NearestNeighbour";
        }
    };
}