#pragma once

#include <algo/ISolution.h>
#include <algorithm>
#include <lib/Timer.h>
#include <random>
#include <lib/ThreadPool.h>

namespace NAlgo {
    class NaiveSolution : public ISolution {
    public:
        explicit NaiveSolution(int version, SolutionConfig config)
                : ISolution(version, std::move(config))
        {}

        Tour solve(const Test& test) override {
            ThreadPool pool(config.thread_count);

            std::vector<std::future<Tour>> results;
            for (int i = 0; i < config.thread_count; i++) {
                results.emplace_back(
                    pool.enqueue([i, &test, deadline=config.deadline](){
                        Timer timer;
                        Tour tour(test);
                        int64_t answer = LONG_LONG_MAX;

                        std::vector<int> v;
                        for(int i = 0; i < test.GetVertexNum(); i++) {
                            v.push_back(i);
                        }

                        std::mt19937 gen(i);

                        timer.Reset();
                        do {
                            Tour candidate(test);
                            candidate.path = v;
                            candidate.CalcTotalWeight();

                            if (candidate.TotalWeight() < answer) {
                                tour = candidate;
                                answer = candidate.TotalWeight();
                            }

                            std::shuffle(v.begin(), v.end(), gen);
                        } while(timer.Passed() < deadline);

                        return tour;
                    })
                );
            }

            Tour tour(test);
            int64_t answer = LONG_LONG_MAX;
            for(auto && result: results) {
                auto candidate = result.get();
                if (candidate.TotalWeight() < answer) {
                    answer = candidate.TotalWeight();
                    tour = candidate;
                }
            }

            return tour;
        }

        std::string solution_name() const override  {
            return "NaiveSolution";
        }
    private:
    };
}




