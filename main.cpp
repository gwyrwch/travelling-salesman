#include <iostream>
#include <cxxopts.hpp>
#include <runner/SolutionsRunner.h>

// ./tsp --mode run-solution --solution-name NearestNeighbour --test-name a280

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
                "test-name",
                "-- choose test to run, default: all",
                ::cxxopts::value<std::string>()->default_value("all")
            );

        auto run_solution_options = solution_opt_parser.parse(argc, argv);

        auto solution_name = run_solution_options["solution-name"].as<std::string>();
        auto test_name = run_solution_options["test-name"].as<std::string>();

        NRunner::SolutionsRunner solutionsRunner(solution_name, test_name);
        solutionsRunner.run_and_save();
    } else {

    }
    return 0;
}
