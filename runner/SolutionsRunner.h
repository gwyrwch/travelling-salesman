#pragma once

#include <algo/SolutionMaker.h>
#include <config/DatasetConfig.h>
#include <lib/Timer.h>

#include <string>
#include <memory>
#include <vector>

namespace NRunner {
    struct TestResult {
        NAlgo::Tour tour;
        std::string solution_name;
        int solution_version;
        double run_time_ms;
    };

    class SolutionsRunner {
    public:
        SolutionsRunner(const std::string& solution_name, std::string test_name)
            : test_name(std::move(test_name))
        {
            solution = NAlgo::MakeSolution(solution_name);
        }

        void run_and_save() {
            save(run());
        }
    private:
        void save(const std::vector<TestResult>& testResults) {
            // TODO узнанть имя файла и сохранить этот результат
        }

        std::vector<TestResult> run() {
            std::vector<TestResult> result;
            if (test_name == "all") {
                auto all_tests = NAlgo::LoadAllTests(NConfig::DatasetConfig::DATASET_LOCATION);

                for (auto test : all_tests) {
                    result.push_back(run_on_single_test(test));
                }
            } else {
                auto test = NAlgo::LoadSingleTest(NConfig::DatasetConfig::DATASET_LOCATION, test_name);
                result.push_back(run_on_single_test(test));
            }
            return result;
        }

        TestResult run_on_single_test(const NAlgo::Test& test) {
            Timer timer;

            auto tour = solution->solve(test);

            return {
                .tour = tour,
                .solution_name = solution->solution_name(),
                .solution_version = solution->solution_version(),
                .run_time_ms = timer.Passed()
            };
        }

        std::unique_ptr<NAlgo::ISolution> solution;
        std::string test_name;
    };
}
