#pragma once

#include <algo/SolutionMaker.h>
#include <algo/Distance.h>
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
            std::string& solution_name,
            std::string _test_name,
            std::optional<std::string> optimizer_name,
            std::optional<NAlgo::OptimizerConfig> optimizer_config,
            std::optional<std::string> comment,
            NAlgo::SolutionConfig solution_config
        )
            : test_name(std::move(_test_name))
            , comment(std::move(comment))
        {
            if (test_name == "all") {
                tests = NAlgo::LoadAllTests(NConfig::DatasetConfig::DATASET_LOCATION);
            } else {
                tests = {NAlgo::LoadSingleTest(NConfig::DatasetConfig::DATASET_LOCATION, test_name)};
            }

            solution = NAlgo::MakeSolution(solution_name, solution_config);
            if (optimizer_name.has_value()) {
                // todo ensure config has value
                optimizer = NAlgo::MakeOptimizer(optimizer_name.value(), optimizer_config.value());
            }
        }

        std::vector<TestResult> run_and_save() {
            auto testResults = run();
            save(testResults);
            return testResults;
        }

        std::vector<TestResult> run_and_save_all() {
            auto testResults = run();
            save(testResults);
            save_convergence(testResults);
            return testResults;
        }

        std::vector<TestResult> run_optimize_and_save() {
            auto test_results = run_and_optimize();
            save(test_results);
            return test_results;
        }

        std::vector<TestResult> run_optimize_and_save_all() {
            auto test_results = run_and_optimize();
            save(test_results);
            save_convergence(test_results);
            return test_results;
        }
    private:
        void save_convergence(const std::vector<TestResult>& testResults) {
            for (auto result : testResults) {
                std::filesystem::path path(
                    NConfig::CacheConfig::CONVERGENCE_DIRECTORY / (
                            result.tour.GetTestName() + "_"
                            + solution->solution_name() + "_"
                            + std::to_string(solution->solution_version())
                            + ".conv"
                    )
                );
                std::ofstream out(path);
                out << "NAME: " << result.tour.GetTestName() + "_"
                                   + result.solution_name << std::endl;
                out << "TYPE: CONVERGENCE" << std::endl;
                out << "DIMENSION: " << result.tour.path.size() << std::endl;
                result.tour.CalcTotalWeight();
                std::cout << result.tour.TotalWeight() << std::endl;
                if (optimizer) {
                    out << "OPTIMIZER: " << optimizer->optimizer_name() << std::endl;
                }

                if (comment.has_value()) {
                    out << "COMMENT: " << comment.value() << std::endl;
                }

                out << "CONVERGENCE_SECTION" << std::endl;

                int delta = result.tour.convergence.size() / 30;
                for (size_t i = 0; i < result.tour.convergence.size(); i += delta) {
                    out << result.tour.convergence[i].first
                        << " "
                        << result.tour.convergence[i].second
                        << std::endl;
                }

                out << -1 << std::endl;
            }
        }

        void save(const std::vector<TestResult>& testResults) {
            for (auto result : testResults) {
                std::filesystem::path path(
                    NConfig::CacheConfig::CACHE_DIRECTORY / (
                        result.tour.GetTestName() + "_" + solution->solution_name() + "_" + std::to_string(solution->solution_version()) + ".tour"
                    )
                );
                std::ofstream out(path);
                out << "NAME: " << result.tour.GetTestName() + "_"
                    + result.solution_name + "_"
                    + std::to_string(result.solution_version) << std::endl;
                out << "TYPE: TOUR" << std::endl;
                out << "DIMENSION: " << result.tour.path.size() << std::endl;
                result.tour.CalcTotalWeight();
                std::cout << result.tour.TotalWeight() << std::endl;
                out << "WEIGHT: " << result.tour.TotalWeight() << std::endl;
                if (optimizer) {
                    out << "OPTIMIZER: " << optimizer->optimizer_name() << std::endl;
                }

                if (comment.has_value()) {
                    out << "COMMENT: " << comment.value() << std::endl;
                }

                out << "TIME: " << result.run_time_ms << std::endl;

                out << "TOUR_SECTION" << std::endl;

                for (auto v : result.tour.path) {
                    out << v << std::endl;
                }

                out << -1 << std::endl;
            }
        }

        std::vector<TestResult> run() {
            std::vector<TestResult> result;
            for (const auto& test : tests) {
                result.push_back(run_on_single_test(test));
                if (test_name == "all") {
                    std::cout << "Ran successfully on test " << test.GetTestName() << ' '
                    << " with score " << result.back().tour.TotalWeight() << std::endl;
                } else {
                    std::cout << "Ran successfully on test " << test_name << ' ' << " with score "
                              << result.back().tour.TotalWeight() << std::endl;
                }
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
                std::cout << "Optimized test " << tests[i].GetTestName() << " new score " << new_tour.TotalWeight() << std::endl;
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
                .run_time_ms = timer.Passed() + (optimizer ? optimizer->get_deadline() : 0)
            };
        }

        NAlgo::Tour optimize_on_single_test(const NAlgo::Test& test, NAlgo::Tour tour)  {
            return optimizer->optimize(test, std::move(tour));
        }

        std::unique_ptr<NAlgo::ISolution> solution;
        std::unique_ptr<NAlgo::IOptimizer> optimizer;
        std::string test_name;
        std::vector<NAlgo::Test> tests;
        std::optional<std::string> comment;
    };
}
