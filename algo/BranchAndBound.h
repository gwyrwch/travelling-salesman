#pragma once

#include <algo/ISolution.h>
#include <lib/Timer.h>

#include <numeric>
#include <algorithm>
#include <random>
#include <iostream>
#include <utility>
#include <vector>

namespace NAlgo {
    class BranchAndBound : public ISolution {
    public:
        explicit BranchAndBound(int version)
            : ISolution(version)
        {}

        Tour solve(const Test& test) override {
            std::vector<int> default_path(test.GetVertexNum());
            std::iota(default_path.begin(), default_path.end(), 0);
            std::mt19937 gen(0);
            std::shuffle(default_path.begin(), default_path.end(), gen);

            best_found = std::make_unique<Tour>(test);
            best_found->path = default_path;
            best_found->CalcTotalWeight();
            current_tour = std::make_shared<Tour>(test);

            used.assign(test.GetVertexNum(), 0);
            timer.Reset();
            kill_dfs = false;
            current_weight = 0;
            order.resize(test.GetVertexNum());
            iter = 0;

            for (int v = 0; v < test.GetVertexNum(); v++) {
                order[v].clear();
                for (int u = 0; u < test.GetVertexNum(); u++) {
                    if (v != u) {
                        auto weight = test.EvalDistance(u, v);
                        order[v].emplace_back(weight, u);
                    }
                }
                std::sort(order[v].begin(), order[v].end());
            }

            dfs(test, test.GetVertexNum());

            Tour result = *best_found;
            result.CalcTotalWeight();
            return result;
        }

        std::string solution_name() const override {
            return "BranchAndBound";
        }
    private:
        int64_t lower_bound(int test_size) {
            int64_t result = 0;
            for (int v = 0; v < test_size; v++) {
                if (used[v] == 0) {
                    int64_t best_weight = LONG_LONG_MAX;

//                    for (auto& [w, u] : order[v]) {
//                        if (u != v && !used[u]) {
//                            best_weight = w;
//                            break;
//                        }
//                    }

                    if (best_weight == LONG_LONG_MAX) {
                        best_weight = order[v][0].first;
                    }

                    assert(best_weight != LONG_LONG_MAX);
                    result += best_weight;
                }
            }
            return result;
        }

        void dfs(const Test& test, int test_size) {
            if (iter % 100000 == 0 && timer.Passed() > deadline) {
                kill_dfs = true;
                return;
            }
            iter += 1;

            if (current_tour->path.size() == test_size) {
                current_tour->CalcTotalWeight();
                if (current_tour->TotalWeight() < best_found->TotalWeight()) {
                    best_found = std::make_unique<Tour>(*current_tour);
                }
                return;
            }

            if (current_weight + lower_bound(test_size) > best_found->TotalWeight()) {
                return;
            }

            int prev = current_tour->path.empty() ? -1 : current_tour->path.back();
            for (int i = 0; i < test_size - 1 && !kill_dfs; i++) {
                int v = (prev == -1) ? i : order[prev][i].second;

                if (used[v] == 0) {
                    used[v] = 1;
                    int64_t weight = 0;
                    if (current_tour->path.size()) {
                        weight = test.EvalDistance(current_tour->path.back(), v);
                    }
                    current_weight += weight;
                    current_tour->path.push_back(v);

                    dfs(test, test_size);

                    current_weight -= weight;
                    current_tour->path.pop_back();
                    used[v] = 0;
                }
            }
        }

        std::vector<char> used;
        std::unique_ptr<Tour> best_found;
        std::shared_ptr<Tour> current_tour;

        Timer timer;
        bool kill_dfs;
        int64_t current_weight;
        std::vector<std::vector<std::pair<int64_t, int>>> order;
        int64_t iter = 0;

        const double deadline = 10000; // todo param
    };
}
