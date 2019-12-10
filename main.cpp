#include <iostream>
#include <cxxopts.hpp>
#include <runner/SolutionsRunner.h>

// ./tsp --mode run-solution --solution-name NearestNeighbour --test-name a280
// ./tsp --mode run-solution --solution-name NearestNeighbour --test-name a280 --optimizer-name LocalSearch

int main(int argc, char** argv) {
    cxxopts::Options opt_parser("TSP solver", "");
    opt_parser.allow_unrecognised_options();

    opt_parser.add_options("Mode chooser")
            ("mode", "-- choose launch mode from (run-solution)", ::cxxopts::value<std::string>());

    auto options = opt_parser.parse(argc, argv);
    auto mode = options["mode"].as<std::string>();

    if (mode == "run-solution") {
        cxxopts::Options solution_opt_parser("", "");
        solution_opt_parser.allow_unrecognised_options();

        solution_opt_parser.add_options("Run options")
            ("solution-name", "-- choose algorithm ", ::cxxopts::value<std::string>())
            (
                "solution-deadline",
                "-- deadline for solution in milliseconds",
                ::cxxopts::value<double>()->default_value("3000")
            )
            (
                "multi",
                "-- if specified, then multi-threaded mode for solution if available"
            )
            (
                "thread-count",
                "-- amount of threads in multithreading mode",
                ::cxxopts::value<int>()->default_value("4")
            )
            (
                "test-name",
                "-- choose test to run, default: all",
                ::cxxopts::value<std::string>()->default_value("all")
            )
            ("optimizer-name", "-- choose optimizer algorithm", ::cxxopts::value<std::string>()->default_value(""))
            (
                    "optimizer-deadline",
                    "-- deadline for optimizer in milliseconds",
                    ::cxxopts::value<double>()->default_value("2000")
            )
            ("comment", "-- write a comment to your solution", ::cxxopts::value<std::string>()->default_value(""));

        auto run_solution_options = solution_opt_parser.parse(argc, argv);

        auto solution_name = run_solution_options["solution-name"].as<std::string>();
        auto solution_deadline = run_solution_options["solution-deadline"].as<double>();
        auto is_multithreaded = run_solution_options["multi"].count() != 0;
        auto thread_count = run_solution_options["thread-count"].as<int>();
        auto test_name = run_solution_options["test-name"].as<std::string>();

        std::optional<std::string> optimizer_name;
        std::optional<NAlgo::OptimizerConfig> optimizer_config;
        if (run_solution_options.count("optimizer-name")) {
            optimizer_name = run_solution_options["optimizer-name"].as<std::string>();
            optimizer_config = NAlgo::OptimizerConfig();
            optimizer_config->deadline = run_solution_options["optimizer-deadline"].as<double>();
        }

        std::optional<std::string> comment;
        if (run_solution_options.count("comment")) {
            comment = run_solution_options["comment"].as<std::string>();
        }


        NRunner::SolutionsRunner solutionsRunner(
            solution_name,
            test_name,
            optimizer_name,
            optimizer_config,
            comment,
            {
                .deadline = solution_deadline,
                .is_multithreaded = is_multithreaded,
                .thread_count = thread_count
            }
        );
        if (optimizer_name.has_value()) {
            solutionsRunner.run_optimize_and_save();
        } else {
            solutionsRunner.run_and_save();
        }
    } else {

    }
    return 0;
}
