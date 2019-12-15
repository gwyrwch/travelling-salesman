#pragma once

#include <algo/IOptimizer.h>
#include <lib/Timer.h>

#include <iostream>
#include <cmath>
#include <random>

namespace NAlgo {
    class SimulatedAnnealing : public IOptimizer {
    public:
        explicit SimulatedAnnealing(int version, OptimizerConfig config)
            : IOptimizer(version, std::move(config))
            , gen(0)
        {}

        Tour optimize(const Test& test, Tour baseline) override {
            double T = 100;
            double alpha = 0.9999;

            baseline.CalcTotalWeight();
            Tour best_tour = baseline;
            Timer timer;

            int64_t currentWeight = baseline.TotalWeight();

            int iter = 0;
            std::vector<std::pair<int, int>> conv;
            if (test.GetVertexNum() < 7500) {
                while (timer.Passed() < config.deadline) {
                    for (int l = 0; l < test.GetVertexNum(); l++)
                        for (int r = l + 1; r < test.GetVertexNum(); r++) {
                            make_iteration(l, r, test, T, baseline, best_tour, currentWeight);

                            if (config.save_method_convergence) {
                                conv.emplace_back(iter, best_tour.TotalWeight());
                            }
                            iter++;
                        }
                    T *= alpha;
                }
            } else {
                while (timer.Passed() < config.deadline) {
                    int l = gen() % (int)test.GetVertexNum();
                    int r = gen() % (int)test.GetVertexNum();

                    if (l > r)
                        std::swap(l, r);

                    make_iteration(l ,r, test, T, baseline, best_tour, currentWeight);

                    T *= alpha;

                    if (config.save_method_convergence) {
                        conv.emplace_back(iter, best_tour.TotalWeight());
                    }
                    iter++;
                }
            }

            best_tour.convergence = conv;
            return best_tour;
        }

        std::string optimizer_name() const override  {
            return "SimulatedAnnealing";
        }
    private:
        std::mt19937 gen;

        double get_random_double() {
            return 1. * gen() / UINT32_MAX;
        }

        void make_iteration(int l ,int r, const Test &test, double T, Tour& baseline, Tour& best_tour, long long& currentWeight) {
            int prev = (l - 1 + test.GetVertexNum()) % test.GetVertexNum();
            int next = (r + 1) % test.GetVertexNum();
            std::vector<int> &p = baseline.path;
            double delta_h = test.EvalDistance(p[prev], p[r]) + test.EvalDistance(p[l], p[next]) -
                             (test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next]));
            double prob = exp(-delta_h / T);
            if (delta_h < 0 || get_random_double() < prob) {
                currentWeight -= test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next]);
                std::reverse(p.begin() + l, p.begin() + r + 1);
                currentWeight += test.EvalDistance(p[prev], p[l]) + test.EvalDistance(p[r], p[next]);
            }

            if (currentWeight < best_tour.TotalWeight()) {
                best_tour = baseline;
                best_tour.CalcTotalWeight();
            }

        }

    };
}
