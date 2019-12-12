#pragma once

#include <algo/ISolution.h>
#include <algorithm>
#include <numeric>
#include <lib/Timer.h>


namespace NAlgo {
    class MinimumSpanningTree : public ISolution {
    public:
        explicit MinimumSpanningTree(int version, SolutionConfig config)
                : ISolution(version, std::move(config))
        {}

        Tour solve(const Test& test) override {
            Tour tour(test);
            int64_t answer = LONG_LONG_MAX;

            std::vector<int> default_path(test.GetVertexNum());
            std::iota(default_path.begin(), default_path.end(), 0);
            tour.path = default_path;
            tour.CalcTotalWeight();
            answer = tour.TotalWeight();

            if (test.GetVertexNum() < 7500) {
                auto min_spanning_tree = find_min_spanning_tree(test, tour);

                for (int i = 0; i < test.GetVertexNum(); i++) {
                    Tour candidate(test);
                    dfs(i, -1, min_spanning_tree, candidate);
                    candidate.CalcTotalWeight();

                    if (candidate.TotalWeight() < answer) {
                        answer = candidate.TotalWeight();
                        tour = candidate;
                    }
                }
            }

            return tour;
        }

        std::string solution_name() const override  {
            return "MinimumSpanningTree";
        }

    private:
        void dfs(int u, int parent, const std::vector<std::vector<int>>& adj_list, Tour& tour) {
            tour.path.push_back(u);
            for (auto v : adj_list[u]) {
                if (v != parent) {
                    dfs(v, u, adj_list, tour);
                }
            }
        }

        std::vector<std::vector<int>> find_min_spanning_tree(const Test& test, const Tour& tour) {
            std::vector<std::tuple<int64_t, int, int> > g;

            for (int i = 0; i < test.GetVertexNum(); i++) {
                for (int j = 0; j < test.GetVertexNum(); j++) {
                    g.emplace_back(test.EvalDistance(i, j), i,  j);
                }
            }

            std::sort(g.begin(), g.end());

            std::vector<int> components;
            components.resize(test.GetVertexNum());
            std::iota(components.begin(), components.end(), 0);

            std::vector<std::vector<int>> adj_list(test.GetVertexNum());

            for (auto [w, i, j] : g) {
                if (components[i] != components[j]) {
                    adj_list[i].push_back(j);
                    adj_list[j].push_back(i);

                    int old_id = components[j],  new_id = components[i];
                    for (auto& c : components)
                        if (c == old_id)
                            c = new_id;
                }
            }

            return adj_list;
        }
    };
}



