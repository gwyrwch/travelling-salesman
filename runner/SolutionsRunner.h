#pragma once

#include <algo/SolutionMaker.h>
#include <config/DatasetConfig.h>
#include <lib/Timer.h>

#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

namespace NRunner {
    struct TestResult {
        NAlgo::Tour tour;
        std::string solution_name;
        int solution_version;
        double run_time_ms;
        std::string optimizer_name;
        int optimizer_version;
    };

    class SolutionsRunner {
    public:
        SolutionsRunner(
            const std::string& solution_name,
            std::string test_name,
            std::optional<std::string> optimizer_name,
            std::optional<NAlgo::OptimizerConfig> optimizer_config
        )
            : test_name(test_name)
        {
            if (test_name == "all") {
                tests = NAlgo::LoadAllTests(NConfig::DatasetConfig::DATASET_LOCATION);
            } else {
                tests = {NAlgo::LoadSingleTest(NConfig::DatasetConfig::DATASET_LOCATION, test_name)};
            }

            solution = NAlgo::MakeSolution(solution_name);
            if (optimizer_name.has_value()) {
                // todo ensure config has value
                optimizer = NAlgo::MakeOptimizer(optimizer_name.value(), optimizer_config.value());
            }
        }

        void run_and_save() {
            save(run());
        }

        void run_optimize_and_save() {
            save(run_and_optimize());
        }
    private:
        void save(const std::vector<TestResult>& testResults) {
            for (auto result : testResults) {
                std::filesystem::path path(
                    NConfig::CacheConfig::CACHE_DIRECTORY / (
                        solution->solution_name() + "_" + std::to_string(solution->solution_version()) + "_" + test_name + ".tour"
                    )
                );
                std::ofstream out(path);
                out << "WEIGHT: " << result.tour.TotalWeight() << std::endl;
                if (optimizer) {
                    out << "OPTIMIZER: " << optimizer->optimizer_name() << std::endl;
                }
            }
        }

        std::vector<TestResult> run() {
            std::vector<TestResult> result;
            for (const auto& test : tests) {
                result.push_back(run_on_single_test(test));
                std::cout << "Ran successfully on test " << test_name << ' ' << " with score " << result.back().tour.TotalWeight() << std::endl;
            }
            return result;
        }

        std::vector<TestResult> run_and_optimize() {
            auto result = run();
            for (size_t i = 0; i < tests.size(); i++) {
                auto new_tour = optimize_on_single_test(tests[i], result[i].tour);
                result[i].tour = new_tour;
                result[i].optimizer_name = optimizer->optimizer_name();
                result[i].optimizer_version = optimizer->optimizer_version();
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

        NAlgo::Tour optimize_on_single_test(const NAlgo::Test& test, NAlgo::Tour tour)  {
            return optimizer->optimize(test, std::move(tour));
        }

        std::unique_ptr<NAlgo::ISolution> solution;
        std::unique_ptr<NAlgo::IOptimizer> optimizer;
        std::string test_name;
        std::vector<NAlgo::Test> tests;
    };
}
