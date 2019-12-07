#pragma once

#include <algo/ISolution.h>
#include <lib/Timer.h>

#include <vector>
#include <numeric>
#include <random>
#include <set>
#include <iostream>

namespace NAlgo {
    enum class ESelectType : int {
        Tournament,
        RouletteMethod,
        Ranking,
        UniformRanking,
        SigmaClipping
    };

    enum class EParentSelectType : int {
        Inbreeding,
        Outbreeding
    };

    struct GAHyperOpt {
        int population_size = 100;
        EParentSelectType parent_select_type = EParentSelectType::Outbreeding;
        ESelectType select_type = ESelectType::Tournament;
        double proportion_of_crossover = 0.3;
        double proportion_of_mutation = 0.04;
        struct RankingOpt {
            double a = 1.5;
            double b = 2 - a;
        } ranking_opt;
    };

    class GeneticAlgorithm : public ISolution {
    public:
        using Path = std::vector<int>;

        explicit GeneticAlgorithm(int version, const GAHyperOpt& hyper_opt = {})
            : ISolution(version)
            , gen(0)
            , hyper_opt(hyper_opt)
        {}

        Tour solve(const Test& test) override {
            Tour tour(test);

            N = hyper_opt.population_size;
            current_population = initialize(test.GetVertexNum());

            timer.Reset();
            while (timer.Passed() < deadline) {
                std::vector<Path> new_population;

                while ((int)new_population.size() + 1 < (int)current_population.size()) {
                    auto p1 = select(hyper_opt.select_type, test, test.GetVertexNum());
                    auto p2 = select(hyper_opt.select_type, test, test.GetVertexNum());

                    bool do_crossover = get_random_double() <= hyper_opt.proportion_of_crossover;
                    bool do_mutate_1 = get_random_double() <= hyper_opt.proportion_of_mutation;
                    bool do_mutate_2 = get_random_double() <= hyper_opt.proportion_of_mutation;

                    if (do_crossover) {
                        auto c1 = cross(p1, p2);
                        auto c2 = cross(p2, p1);
                        p1 = c1;
                        p2 = c2;
                    }

                    if (do_mutate_1) {
                        mutate(p1, test.GetVertexNum());
                    }
                    if (do_mutate_2) {
                        mutate(p2, test.GetVertexNum());
                    }

                    new_population.push_back(p1);
                    new_population.push_back(p2);
                }

                current_population = new_population;
                N = current_population.size();

                std::vector<int64_t> all_weights;
                Tour candidate(test);
                for (int i = 0; i < current_population.size(); i++) {
                    candidate.path = current_population[i];
                    candidate.CalcTotalWeight();
                    all_weights.push_back(candidate.TotalWeight());
                    if (candidate.TotalWeight() < tour.TotalWeight()) {
                        tour = candidate;
                    }
                }
                std::sort(all_weights.begin(), all_weights.end());
                for (int i = 0; i < 1; i++) {
                    std::cout << all_weights[i] << ' ';
                }
                std::cout << std::endl;
            }

            tour.CalcTotalWeight();
            assert(tour.TotalWeight() != LONG_LONG_MAX);
            return tour;
        }

        std::string solution_name() const override  {
            return "GeneticAlgorithm";
        }


    private:
        std::vector<Path> current_population;
        int N;

        GAHyperOpt hyper_opt;

        std::mt19937 gen;
        Timer timer;
        const double deadline = 5000;

        std::vector<Path> initialize(int vertex_num) {
            std::vector<Path> population;
            population.resize(N);

            Path default_path(vertex_num);
            std::iota(default_path.begin(), default_path.end(), 0);

            for (int i = 0; i < N; i++) {
                population[i] = default_path;
                std::shuffle(population[i].begin() + 1, population[i].end(), gen);
            }
            return population;
        }

        Path select(ESelectType select_type, const Test& test, int vertex_num) {
            if (select_type == ESelectType::Tournament) {
                int i = gen() % N;
                int j = gen() % N;

                Tour tour_i(test), tour_j(test);

                for (int k = 0; k < vertex_num; k++) {
                    tour_i.path.push_back(current_population[i][k]);
                    tour_j.path.push_back(current_population[j][k]);
                }

                tour_i.CalcTotalWeight();
                tour_j.CalcTotalWeight();

                if (tour_i.TotalWeight() > tour_j.TotalWeight()) {
                    return current_population[j];
                } else {
                    return current_population[i];
                }
            } else if (select_type == ESelectType::RouletteMethod || select_type == ESelectType::Ranking) {
                int64_t total_weight_for_all = 0;
                std::vector<int64_t> total_weights;
                Tour tour(test);

                for (const auto& ind : current_population) {
                    tour.path = ind;
                    tour.CalcTotalWeight();
                    total_weights.push_back(tour.TotalWeight());
                    total_weight_for_all += total_weights.back();
                }

                std::vector<double> probs;
                for (int i = 0; i < N; i++) {
                    probs.emplace_back(std::log(1. * total_weights[i] / total_weight_for_all));
                }

                auto log_sum = std::accumulate(probs.begin(), probs.end(), 0.0);
                for (int i = 0; i < N; i++) {
                    probs[i] = probs[i] / log_sum;
                }

                if (select_type == ESelectType::Ranking) {
                    std::vector<int> index;
                    index.resize(N);
                    std::iota(index.begin(), index.end(), 0);

                    std::sort(index.begin(), index.end(), [&probs](int i, int j) {
                        return probs[i] < probs[j];
                    });

                    std::vector<int> order(N);
                    for (int i = 0; i < N; i++) {
                        order[index[i]] = i;
                    }

                    for (int i = 0; i < N; i++) {
                        probs[i] = 1. / N * (hyper_opt.ranking_opt.a -
                                (hyper_opt.ranking_opt.a - hyper_opt.ranking_opt.b) * order[i] / (N - 1));
                    }
                }

                auto rnd = get_random_double();
                double sum = 0;
                int to_kill = -1;
                for (int i = 0; i < N; i++) {
                    double prob = probs[i];
                    sum += prob;

                    if (rnd < sum + 1e-9) {
                        to_kill = i;
                        break;
                    }
                }

                assert(to_kill != -1);
                return current_population[to_kill];
            } else if (select_type == ESelectType::UniformRanking) {

            } else if (select_type == ESelectType::SigmaClipping) {

            }

            return {};
        }

//        std::pair<std::vector<int>, std::vector<int>> choose_parents(EParentSelectType select_type, int count_children) {
//            if (select_type == EParentSelectType::Inbreeding || select_type == EParentSelectType::Outbreeding) {
//                std::vector<int> parents1, parents2;
//                for (int iter = 0; iter < count_children; iter++) {
//                    int par1 = gen() % N;
//                    int par2 = 0;
//                    int best_count = select_type == EParentSelectType::Inbreeding ? -1 : INT_MAX;
//
//                    for (int i = 0; i < N; i++) {
//                        if (i != par1) {
//                            int count = 0;
//                            for (int j = 0; j < N; j++) {
//                                count += current_population[par1][j] == current_population[i][j];
//                            }
//                            if (
//                                (select_type == EParentSelectType::Inbreeding && count > best_count) ||
//                                (select_type == EParentSelectType::Outbreeding && count < best_count)
//                            ){
//                                best_count = count;
//                                par2 = i;
//                            }
//                        }
//                    }
//
//                    parents1.push_back(par1);
//                    parents2.push_back(par2);
//                }
//                return {parents1, parents2};
//            }
//
//            return {};
//        }

        Path cross(const Path& first_parent, const Path& second_parent) {
            std::vector<char> used(first_parent.size(), 0);

            auto gen_bitmask = [this](int n) {
                std::vector<char> res(n);
                for (int i = 0; i < n; i++) {
                    res[i] = gen() % 2;
                }
                return res;
            };

            auto bitmask = gen_bitmask(first_parent.size());

            Path child(first_parent.size(), -1);
            int tf1 = 0, tf2 = 0, tf3 = 0;

            for (int i = 0; i < first_parent.size(); i++) {
                if (bitmask[i]) {
                    used[first_parent[i]] = 1;
                    child[i] = first_parent[i];
                }
            }

            for (int i = 0; i < second_parent.size(); i++) {
                if (child[i] == -1 && !used[second_parent[i]]) {
                    used[second_parent[i]] = 1;
                    child[i] = second_parent[i];
                }
            }

            Path rest;
            for (int i = 0; i < first_parent.size(); i++) {
                if (!used[first_parent[i]]) {
                    rest.push_back(first_parent[i]);
                }
            }

            for (int i = 0, j = 0; i < first_parent.size(); i++) {
                if (child[i] == -1) {
                    child[i] = rest[j++];
                }
            }

            return child;
        }

        void mutate(Path& p, int vertex_num) {
            int l = gen() % vertex_num;
            int r = gen() % vertex_num;

            if (r < l) {
                std::swap(l, r);
            }

            if (l == 0) {
                return;
            }

            std::reverse(p.begin() + l, p.begin() + r + 1);
        }

        double get_random_double() {
            return 1. * gen() / UINT32_MAX;
        }
    };
}

