#include <iostream>
#include <cxxopts.hpp>
#include <runner/SolutionsRunner.h>
#include <util/String.h>

#define __USE_GTK3__

#include <boost/python.hpp>
#include <pygobject.h>
#include <gtkmm.h>

using namespace boost::python;
using namespace std;

// ./tsp --mode run-solution --solution-name NearestNeighbour --test-name a280
// ./tsp --mode run-solution --solution-name NearestNeighbour --test-name a280 --optimizer-name LocalSearch
std::string read_exact_weight(std::string test_name) {
    std::filesystem::path p = "./exact_tour_weights";
    std::ifstream file_in(p);

    if (!file_in) {
        throw std::runtime_error("no such file");
    }

    std::string line;
    while (true) {
        getline(file_in, line);
        if (line == "-1") {
            break;
        }

        auto tokens = NUtil::SplitAndTrim(line, ':');
        if (tokens[0] == test_name) {
            return tokens[1];
        }
    }

    return "";
}


void DisplayInterface(std::vector<NRunner::TestResult> results, std::string exact_weight) {
    try {
        Py_Initialize();
        PyRun_SimpleString("import signal");
        PyRun_SimpleString("signal.signal(signal.SIGINT, signal.SIG_DFL)");

        PyRun_SimpleString("import matplotlib\n"
                           "matplotlib.use('GTK3Agg')");
        PyRun_SimpleString("import matplotlib.pyplot as plt");
        PyRun_SimpleString("fig, ax = plt.subplots(dpi=170)");
        if (results.size() == 1) {
            auto tour = results[0].tour;
            std::string new_x, new_y;

            auto test = tour.GetTest();
            for (int i = 0; i <= tour.path.size(); i++) {
                new_x += std::to_string(test.GetPoint(tour.path[i % tour.path.size()]).x) + ",";
                new_y += std::to_string(test.GetPoint(tour.path[i % tour.path.size()]).y) + ",";
            }

            std::string plot_cmd = "plt.plot([" + new_x + "]" + ", [" + new_y + "], "
                                   "linewidth=0.4, marker='.', markersize=1, "
                                   "color=\"#673ab7\",markeredgecolor=\"black\")";
            PyRun_SimpleString(plot_cmd.c_str());
            PyRun_SimpleString("plt.savefig('fig.png')");
        }
        Py_Finalize();

        Gtk::Main kit(0,0);

        Gtk::Window window;
        window.set_title("TSP");
        window.set_default_size(2400, 1400);

        Gtk::ScrolledWindow scrolledWindow;
        window.add(scrolledWindow);

        Gtk::Fixed fixed;
        scrolledWindow.add(fixed);

        Gtk::Label label;
        label.set_text(
            "found: "
            + std::to_string(results[0].tour.TotalWeight())
            + "\n"
            + "exact: "
            + exact_weight
        );
        label.set_size_request(100, 20);
        fixed.add(label);
        fixed.move(label, 0, 0);

        Gtk::Image img("./fig.png");
        fixed.add(img);
        fixed.move(img, 0, 35);

        window.show_all();

        Gtk::Main::run(window);
    } catch( error_already_set ) {
        PyErr_Print();
    }
}


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
            ("solution-name",
                    "-- choose algorithm ",
                    ::cxxopts::value<std::string>()
            )
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
                ::cxxopts::value<int>()->default_value("1")
            )
            (
                "test-name",
                "-- choose test to run, default: all",
                ::cxxopts::value<std::string>()->default_value("all")
            )
            (
                "optimizer-name",
                "-- choose optimizer algorithm",
                ::cxxopts::value<std::string>()->default_value("")
            )
            (
                "optimizer-deadline",
                "-- deadline for optimizer in milliseconds",
                ::cxxopts::value<double>()->default_value("2000")
            )
            (
                "comment",
                "-- write a comment to your solution",
                ::cxxopts::value<std::string>()->default_value("")
            )
            (
                "save-convergence",
                "-- if specified, then convergence of the method is saved to file"
            );

        auto run_solution_options = solution_opt_parser.parse(argc, argv);

        auto solution_name = run_solution_options["solution-name"].as<std::string>();
        auto solution_deadline = run_solution_options["solution-deadline"].as<double>();
        auto thread_count = run_solution_options["thread-count"].as<int>();
        auto test_name = run_solution_options["test-name"].as<std::string>();
        auto save_method_convergence = run_solution_options["save-convergence"].count() != 0;

        std::optional<std::string> optimizer_name;
        std::optional<NAlgo::OptimizerConfig> optimizer_config;
        if (run_solution_options.count("optimizer-name")) {
            optimizer_name = run_solution_options["optimizer-name"].as<std::string>();
            optimizer_config = NAlgo::OptimizerConfig();
            optimizer_config->deadline = run_solution_options["optimizer-deadline"].as<double>();
            optimizer_config->save_method_convergence = save_method_convergence;
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
                .thread_count = thread_count,
                .save_method_convergence = save_method_convergence
            }
        );

        std::vector<NRunner::TestResult> runResults;
        if (optimizer_name.has_value() && save_method_convergence) {
            runResults = solutionsRunner.run_optimize_and_save_all();
        } else if(optimizer_name.has_value()) {
            runResults = solutionsRunner.run_optimize_and_save();
        } else if(save_method_convergence) {
            runResults = solutionsRunner.run_and_save_all();
        } else {
            runResults = solutionsRunner.run_and_save();
        }

        if (runResults.size() == 1) {
            std::string exact_weight = read_exact_weight(runResults[0].tour.GetTestName());
            if (exact_weight == "")
                throw std::runtime_error("no test with such name");

            DisplayInterface(runResults, exact_weight);
        }

    } else if (mode == "list-solutions"){
        std::vector<std::string> all_solutions = {
            "NaiveSolution",
            "NearestNeighbour",
            "BranchAndBound",
            "MinimumSpanningTree",
            "GeneticAlgorithm"
        };

        std::cout << "Available solutions: " << std::endl;
        for (auto s : all_solutions)
            std::cout << s << std::endl;

    } else if (mode == "list-optimizers"){
        std::vector<std::string> all_optimizers = {
            "LocalSearch",
            "SimulatedAnnealing"
        };

        std::cout << "Available optimizers: " << std::endl;
        for (auto opt : all_optimizers)
            std::cout << opt << std::endl;
    }
    return 0;
}
