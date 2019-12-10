#pragma once

#include <algo/ISolution.h>
#include <lib/Timer.h>
#include <lib/ThreadPool.h>

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
        Outbreeding,
        Nothing
    };

    struct GAHyperOpt {
        int population_size = 100;
        EParentSelectType parent_select_type = EParentSelectType::Nothing;
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

        explicit GeneticAlgorithm(int version, SolutionConfig config, const GAHyperOpt& hyper_opt = {})
            : ISolution(version, std::move(config))
            , gen(0)
            , hyper_opt(hyper_opt)
        {}

        std::pair<Path, Path> genetic_epoch(const Test& test) {
            auto p1 = select(hyper_opt.select_type, test, test.GetVertexNum());
            auto p2 = hyper_opt.parent_select_type == EParentSelectType::Nothing
                      ? select(hyper_opt.select_type, test, test.GetVertexNum())
                      : choose_parent(hyper_opt.parent_select_type, p1);

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

            return {p1, p2};
        }

        Tour solve(const Test& test) override {
            Tour tour(test);

            current_population = initialize(hyper_opt.population_size, test.GetVertexNum());

            ThreadPool pool(config.thread_count);
            timer.Reset();

            while (timer.Passed() < config.deadline) {
                std::vector<Path> new_population;

                if (config.is_multithreaded) {
                    size_t pool_size = current_population.size() / 2;
                    std::vector<std::future<std::pair<Path, Path>>> results;

                    for(size_t i = 0; i < pool_size; i++) {
                        results.emplace_back(
                            pool.enqueue([this, &test](){
                                auto [p1, p2] = genetic_epoch(test);
                                return std::make_pair(p1, p2);
                            })
                        );
                    }

                    for(auto && result: results) {
                        auto [p1, p2] = result.get();
                        new_population.push_back(p1);
                        new_population.push_back(p2);
                    }
                } else {
                    while ((int) new_population.size() + 1 < (int) current_population.size()) {
                        auto [p1, p2] = genetic_epoch(test);

                        new_population.push_back(std::move(p1));
                        new_population.push_back(std::move(p2));
                    }
                }
                current_population = std::vector<Path>(new_population.begin(), new_population.end());
            }

            std::vector<int64_t> all_weights;
            Tour candidate(test);
            for (size_t i = 0; i < current_population.size(); i++) {
                candidate.path = current_population[i];
                candidate.CalcTotalWeight();
                all_weights.push_back(candidate.TotalWeight());
                if (candidate.TotalWeight() < tour.TotalWeight()) {
                    tour = candidate;
                }
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

        GAHyperOpt hyper_opt;

        std::mt19937 gen;
        Timer timer;

        std::vector<Path> initialize(int count, int vertex_num) {
            std::vector<Path> population;
            population.resize(count);

            Path default_path(vertex_num);
            std::iota(default_path.begin(), default_path.end(), 0);

            for (int i = 0; i < count; i++) {
                population[i] = default_path;
                std::shuffle(population[i].begin() + 1, population[i].end(), gen);
            }
            return population;
        }

        Path select(ESelectType select_type, const Test& test, int vertex_num) {
            if (select_type == ESelectType::Tournament) {
                int i = gen() % (int)current_population.size();
                int j = gen() % (int)current_population.size();

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

                auto mn_weight = *std::min_element(total_weights.begin(), total_weights.end());
                auto mx_weight = *std::max_element(total_weights.begin(), total_weights.end());
                std::vector<long double> probs;
                for (size_t i = 0; i < current_population.size(); i++) {
                    probs.emplace_back(std::log(1. * (total_weights[i] - mn_weight + 1) / (mx_weight - mn_weight + 1)));
                }

                auto log_sum = std::accumulate(probs.begin(), probs.end(), (long double)0);
                for (size_t i = 0; i < current_population.size(); i++) {
                    if (mn_weight == mx_weight) {
                        probs[i] = (long double)1 / current_population.size();
                    } else {
                        probs[i] = probs[i] / log_sum;
                    }
                }

                if (select_type == ESelectType::Ranking) {
                    std::vector<int> index;
                    index.resize(current_population.size());
                    std::iota(index.begin(), index.end(), 0);

                    std::sort(index.begin(), index.end(), [&probs](int i, int j) {
                        return probs[i] < probs[j];
                    });

                    std::vector<int> order(current_population.size());
                    for (size_t i = 0; i < current_population.size(); i++) {
                        order[index[i]] = i;
                    }

                    for (size_t i = 0; i < current_population.size(); i++) {
                        probs[i] = 1. / current_population.size() * (hyper_opt.ranking_opt.a -
                                (hyper_opt.ranking_opt.a - hyper_opt.ranking_opt.b) * order[i] / (1. * current_population.size() - 1));
                    }
                }

                auto rnd = get_random_double();
                double sum = 0;
                int to_select = -1;
                for (size_t i = 0; i < current_population.size(); i++) {
                    double prob = probs[i];
                    sum += prob;

                    if (rnd < sum + 1e-9) {
                        to_select = i;
                        break;
                    }
                }

                assert(to_select != -1);
                return current_population[to_select];
            } else if (select_type == ESelectType::UniformRanking) {

            } else if (select_type == ESelectType::SigmaClipping) {

            }

            return {};
        }

        Path choose_parent(EParentSelectType select_type, const Path& p1) {
            if (select_type == EParentSelectType::Inbreeding || select_type == EParentSelectType::Outbreeding) {
                int par2 = 0;
                int best_count = select_type == EParentSelectType::Inbreeding ? -1 : INT_MAX;

                for (size_t i = 0; i < current_population.size(); i++) {
                    if (current_population[i] != p1) {
                        int count = 0;
                        for (size_t j = 0; j < current_population.size(); j++) {
                            count += p1[j] == current_population[i][j];
                        }
                        if (
                            (select_type == EParentSelectType::Inbreeding && count > best_count) ||
                            (select_type == EParentSelectType::Outbreeding && count < best_count)
                        ){
                            best_count = count;
                            par2 = i;
                        }
                    }
                }
                return current_population[par2];
            }

            return {};
        }

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

            for (size_t i = 0; i < first_parent.size(); i++) {
                if (bitmask[i]) {
                    used[first_parent[i]] = 1;
                    child[i] = first_parent[i];
                }
            }

            for (size_t i = 0; i < second_parent.size(); i++) {
                if (child[i] == -1 && !used[second_parent[i]]) {
                    used[second_parent[i]] = 1;
                    child[i] = second_parent[i];
                }
            }

            Path rest;
            for (size_t i = 0; i < first_parent.size(); i++) {
                if (!used[first_parent[i]]) {
                    rest.push_back(first_parent[i]);
                }
            }

            for (size_t i = 0, j = 0; i < first_parent.size(); i++) {
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

