#pragma once

#include <algo/IOptimizer.h>
#include <lib/Timer.h>

#include <iostream>

namespace NAlgo {
    class LocalSearch : public IOptimizer {
    public:
        explicit LocalSearch(int version, OptimizerConfig config)
            : IOptimizer(version, std::move(config))
        {}

        Tour optimize(const Test& test, Tour baseline) override {
            baseline.CalcTotalWeight();
            Tour best_tour = baseline;
            Timer timer;

            int64_t currentWeight = baseline.TotalWeight();

            while (timer.Passed() < config.deadline) {
                for (int l = 0; l < test.GetVertexNum(); l++)
                    for (int r = l + 1; r < test.GetVertexNum(); r++) {
                        int prev = (l - 1 + test.GetVertexNum()) % test.GetVertexNum();
                        int next = (r + 1) % test.GetVertexNum();
                        std::vector<int>& p = baseline.path;
                        if (
                            test.EvalDistance(p[prev], p[r]) + test.EvalDistance(p[l], p[next]) <
                            test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next])
                        ) {
                            currentWeight -= test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next]);
                            std::reverse(p.begin() + l, p.begin() + r + 1);
                            currentWeight += test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next]);
                        }
                        if (currentWeight < best_tour.TotalWeight()) {
                            best_tour = baseline;
                            best_tour.CalcTotalWeight();
                            assert(best_tour.TotalWeight() == currentWeight); // fixme checking for first time
                        }
                    }
            }

            return best_tour;
        }

        std::string optimizer_name() const override  {
            return "LocalSearch";
        }
    private:

    };
}
