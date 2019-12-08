#pragma once

#include <algo/Tour.h>
#include <string>

namespace NAlgo {
    struct SolutionConfig {
        double deadline;
        bool is_multithreaded;
    };

    class ISolution {
    public:
        explicit ISolution(int version, SolutionConfig config)
            : version(version)
            , config(std::move(config))
        {}
        virtual ~ISolution() = default;

        virtual Tour solve(const Test& test) = 0;

        /*
            Возвращает название алгоритма решения.
         */
        virtual std::string solution_name() const = 0;

        int solution_version() const {
            return version;
        }
    protected:
        int version;
        SolutionConfig config;
    };

    enum class ESolution : int {
        NearestNeighbour,
        NaiveSolution,
        MinimumSpanningTree,
        BranchAndBound,
        GeneticAlgorithm,

        INVALID_SOLUTION_NAME
    };
}
